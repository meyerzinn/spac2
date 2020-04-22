#include "NetworkingSystem.h"
#include "Constants.h"
#include "EntityComponents.h"
#include "NetworkingComponents.h"
#include "PhysicsComponents.h"
#include "ShipComponents.h"
#include "packet_generated.h"

#include <flatbuffers/flatbuffers.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_set>

using namespace std::chrono;

namespace spac::server::system {

template <bool SSL>
NetworkingSystem<SSL>::NetworkingSystem(entt::registry &registry, b2World &world, uWS::Loop *loop)
    : System(registry),
      mWorld(world),
      mLoop(loop),
      mDeathObserver{registry,
                     entt::collector.group<component::NetClient<SSL>>(entt::exclude<component::ShipController>)} {}

template <bool SSL>
void NetworkingSystem<SSL>::update() {
  // Execute all tasks that have been queued -- this is where networking callbacks are allowed to interact with the
  // registry.
  QueuedTask *t = nullptr;
  while (mTaskQueue.pop(t)) {
    (*t)();
    delete t;
  }

  auto now = high_resolution_clock::now();

  // Send death messages
  for (auto client : mDeathObserver) {
    auto &net = mRegistry.get<component::NetClient<SSL>>(client);
    if (net.spawned.time_since_epoch().count() > 0) {  // make sure the client didn't just connect
      milliseconds timeAlive = duration_cast<milliseconds>(now - net.spawned);

      flatbuffers::FlatBufferBuilder builder(64);  // todo set to exact size
      auto death = net::CreateDeath(builder, timeAlive.count());
      auto packet = net::CreatePacket(builder, net::Message_Death, death.Union());
      net::FinishPacketBuffer(builder, packet);
      std::string_view buffer(reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
      mLoop->defer([&net, buffer]() { net.conn->send(buffer); });
    }
  }
  mDeathObserver.clear();
}

template <bool SSL>
void NetworkingSystem<SSL>::listen(int port, uWS::TemplatedApp<SSL> app) {
  using namespace std::placeholders;
  app.get("/hello", [](auto *res,
                       auto *req) { res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("Hello HTTP!"); })
      .template ws<SocketData>("/play", {.compression = uWS::SHARED_COMPRESSOR,
                                         .maxPayloadLength = 1024,
                                         .idleTimeout = 0,
                                         .maxBackpressure = 1 * 1024 * 1024,
                                         .open = std::bind(&NetworkingSystem<SSL>::onOpen, this, _1, _2),
                                         .message = std::bind(&NetworkingSystem<SSL>::onMessage, this, _1, _2, _3),
                                         .drain = std::bind(&NetworkingSystem<SSL>::onDrain, this, _1),
                                         .ping = std::bind(&NetworkingSystem<SSL>::onPing, this, _1),
                                         .pong = std::bind(&NetworkingSystem<SSL>::onPong, this, _1),
                                         .close = std::bind(&NetworkingSystem<SSL>::onClose, this, _1, _2, _3)})
      .listen("0.0.0.0", port,
              [port](auto *token) {
                if (!token) {
                  std::cout << "No token!" << std::endl;
                }
                std::cout << "Listening on port " << port << std::endl;
              })
      .run();
}

template <bool SSL>
void NetworkingSystem<SSL>::onOpen(WebSocket *ws, uWS::HttpRequest *req) {
  std::cout << "connected" << std::endl;
  QueuedTask tsk = [this, ws]() {
    auto id = mRegistry.create();
    auto data = (SocketData *)ws->getUserData();
    auto netClient = mRegistry.assign<component::NetClient<SSL>>(id, ws);
    data->closed = netClient.closed;
  };
  while (!mTaskQueue.push(&tsk)) {
  }
}

template <bool SSL>
void NetworkingSystem<SSL>::onMessage(WebSocket *ws, std::string_view message, uWS::OpCode opCode) {
  auto verifier = flatbuffers::Verifier(reinterpret_cast<const uint8_t *>(message.data()), message.size());
  if (!net::VerifyPacketBuffer(verifier)) {
    std::cout << "Received malformed packet from client!" << std::endl;
    // todo handle more gracefully
  }
  const net::Packet *packet = net::GetPacket(reinterpret_cast<const uint8_t *>(message.data()));
}

template <bool SSL>
void NetworkingSystem<SSL>::onDrain(WebSocket *ws) {
  // todo idk what do do on drain-- need to relieve some backpressure I suppose?
}

template <bool SSL>
void NetworkingSystem<SSL>::onPing(WebSocket *ws) {
  std::cout << "Received ping." << std::endl;
}

template <bool SSL>
void NetworkingSystem<SSL>::onPong(WebSocket *ws) {
  std::cout << "Received pong." << std::endl;
}

template <bool SSL>
void NetworkingSystem<SSL>::onClose(WebSocket *ws, int code, std::string_view message) {
  auto data = (SocketData *)ws->getUserData();
  *(data->closed.get()) = false;
  std::cout << "Client connection closed." << std::endl;
}

template class NetworkingSystem<true>;

template class NetworkingSystem<false>;
}  // namespace spac::server::system