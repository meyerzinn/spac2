#include "NetworkingSystem.h"


namespace spac::server::system {

constexpr float SHIP_DENSITY = 7900;
constexpr float PERCEPTION_RADIUS = 150;

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
      .template ws<SocketData>("/play",
                               {.compression = uWS::SHARED_COMPRESSOR,
                                .maxPayloadLength = 1024,
                                .idleTimeout = 0,
                                .maxBackpressure = 1 * 1024 * 1024,
                                .open = [this](auto &&PH1, auto &&PH2) { onOpen(PH1, PH2); },
                                .message = [this](auto &&PH1, auto &&PH2, auto &&PH3) { onMessage(PH1, PH2, PH3); },
                                .drain = [this](auto &&PH1) { onDrain(PH1); },
                                .ping = [this](auto &&PH1) { onPing(PH1); },
                                .pong = [this](auto &&PH1) { onPong(PH1); },
                                .close = [this](auto &&PH1, auto &&PH2, auto &&PH3) { onClose(PH1, PH2, PH3); }})
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
    std::cout << "Received malformed packet from client." << std::endl;
    return;
    // todo handle more gracefully
  }
  SocketData *socketData = reinterpret_cast<SocketData *>(ws->getUserData());
  const net::Packet *packet = net::GetPacket(reinterpret_cast<const uint8_t *>(message.data()));
  switch (packet->message_type()) {
    case net::Message_Respawn:
      handleRespawn(socketData->id, packet);
      break;
    default:
      std::cout << "Received packet with improper message type from client." << std::endl;
      break;
  }
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

template <bool SSL>
void NetworkingSystem<SSL>::handleRespawn(entt::entity entity, const net::Packet *packet) {
  auto name = packet->message_as_Respawn()->name()->str();
  mLoop->defer([this, entity, name]() {
    if (!mRegistry.has<component::Named>(entity)) {
      // spawn player
      // todo choose spawn position more intelligently
      auto spawnPosition = b2Vec2_zero;

      mRegistry.assign<component::Named>(entity, name);
      mRegistry.assign<component::Owner>(entity);
      mRegistry.assign<component::ShipController>(entity);
      mRegistry.assign<component::Health>(entity, 100);
      // todo create fuel fixture
      mRegistry.assign<component::Fuel>(entity);
      mRegistry.assign<component::Booster>(entity);
      mRegistry.assign<component::Shielded>(entity);
      mRegistry.assign<component::SideBooster>(entity);
      mRegistry.assign<component::DealsDamage>(entity, 0.1);
      mRegistry.assign<component::Perceivable>(entity, component::Perceivable::Kind::SHIP);
      mRegistry.assign<component::Sensing>(entity);

      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position.Set(spawnPosition.x, spawnPosition.y);
      bodyDef.allowSleep = true;
      bodyDef.awake = true;
      auto *id = new entt::entity(entity);
      bodyDef.userData = reinterpret_cast<void *>(id);
      b2Body *body = mWorld.CreateBody(&bodyDef);

      // todo add shield sensor fixture
      b2CircleShape sensorShape;
      sensorShape.m_radius = PERCEPTION_RADIUS;

      b2FixtureDef sensorFixtureDef;
      sensorFixtureDef.isSensor = true;
      sensorFixtureDef.userData = reinterpret_cast<void *>(id);
      sensorFixtureDef.filter.maskBits = CollisionMask::HEALTH | CollisionMask::DAMAGER;
      sensorFixtureDef.filter.categoryBits = CollisionMask::SENSOR;

      b2Vec2 vertices[3];
      vertices[0].Set(0, 5.0 - 5.0 / 3.0);
      vertices[1].Set(-2, -5.0 / 3.0);
      vertices[2].Set(2, -5.0 / 3.0);

      int32_t count = 3;
      b2PolygonShape polygonShape;
      polygonShape.Set(vertices, count);

      b2FixtureDef fixtureDef;
      fixtureDef.userData = reinterpret_cast<void *>(id);
      fixtureDef.filter.maskBits = CollisionMask::HEALTH | CollisionMask::DAMAGER | CollisionMask::SENSOR;
      fixtureDef.filter.categoryBits = CollisionMask::HEALTH | CollisionMask::DAMAGER;
      fixtureDef.shape = &polygonShape;
      fixtureDef.density = SHIP_DENSITY;
      fixtureDef.restitution = 0.8;  // todo tune restitution
      mRegistry.assign<component::PhysicsBody>(entity, body);
    }
  });
}

template class NetworkingSystem<true>;

template class NetworkingSystem<false>;
}  // namespace spac::server::system