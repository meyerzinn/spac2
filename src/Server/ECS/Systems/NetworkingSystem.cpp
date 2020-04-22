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
  ;
  app.get("/hello", [](auto *res,
                       auto *req) { res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("Hello HTTP!"); })
      .template ws<SocketData>(
          "/play", {.compression = uWS::SHARED_COMPRESSOR,
                    .maxPayloadLength = 1024,
                    .idleTimeout = 0,
                    .maxBackpressure = 1 * 1024 * 1024,
                    .open =
                        [this](uWS::WebSocket<SSL, true> *ws, uWS::HttpRequest *req) {
                          std::cout << "connected" << std::endl;
                          QueuedTask tsk = [this, ws]() {
                            auto id = mRegistry.create();
                            auto data = (SocketData *)ws->getUserData();
                            auto netClient = mRegistry.assign<component::NetClient<SSL>>(id, ws);
                            data->closed = netClient.closed;
                          };
                          while (!mTaskQueue.push(&tsk)) {
                          }
                        },
                    .message =
                        [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                          std::cout << "message: " << message << std::endl;
                          ws->send(message, opCode);  // this echoes?
                        },
                    .drain = [](auto *ws) { std::cout << "Drain?" << std::endl; },
                    .ping =
                        [](auto *ws) {
                          // respond with pong?
                          std::cout << "Received ping." << std::endl;
                        },
                    .pong = [](auto *ws) { std::cout << "Received pong." << std::endl; },
                    .close = [](uWS::WebSocket<SSL, true> *ws, int code,
                                std::string_view message) { std::cout << "Connection closed." << std::endl; }})
      .listen("0.0.0.0", port, [port](auto *token) {
        if (!token) {
          std::cout << "No token!" << std::endl;
        }
        std::cout << "Listening on port " << port << std::endl;
      });
}

template class NetworkingSystem<true>;

template class NetworkingSystem<false>;
}  // namespace spac::server::system