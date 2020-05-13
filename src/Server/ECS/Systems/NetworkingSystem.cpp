#include "NetworkingSystem.h"
#include <box2d/box2d.h>
#include <boost/log/trivial.hpp>
#include "CollisionFlags.h"
#include "EntityComponents.h"
#include "PerceptionComponents.h"
#include "PhysicsComponents.h"
#include "ShipComponents.h"

namespace spac::server::system {

constexpr float SHIP_DENSITY = 7900;
constexpr float PERCEPTION_RADIUS = 150;

void fail(beast::error_code ec, std::string what) {
  BOOST_LOG_TRIVIAL(error) << what << ": " << ec.message() << std::endl;
}

NetworkingSystem::NetworkingSystem(entt::registry &registry, b2World &world, asio::io_context &ioc, ssl::context &ctx,
                                   tcp::endpoint endpoint)
    : System(registry),
      world_(world),
      deathObserver_{
          registry,
          entt::basic_collector<>::group<component::SessionComponent>(entt::exclude<component::ShipController>),
      },
      ioc_(ioc),
      ctx_(ctx),
      acceptor_(asio::make_strand(ioc)) {
  beast::error_code ec;
  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
    return;
  }
  BOOST_LOG_TRIVIAL(debug) << "open" << std::endl;

  acceptor_.set_option(asio::socket_base::reuse_address(false), ec);
  if (ec) {
    fail(ec, "set_option");
    return;
  }

  BOOST_LOG_TRIVIAL(debug) << "set_option" << std::endl;

  acceptor_.bind(endpoint, ec);
  if (ec) {
    fail(ec, "bind");
    return;
  }

  BOOST_LOG_TRIVIAL(debug) << "bind" << std::endl;

  acceptor_.listen(asio::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
    return;
  }

  BOOST_LOG_TRIVIAL(debug) << "listen" << std::endl;
}

void NetworkingSystem::update() {
  auto now = high_resolution_clock::now();

  auto clients = registry_.view<component::SessionComponent>();
  std::string buffer;
  for (entt::entity client : clients) {
    auto conn = clients.get(client).conn;
    // We only pull one message off the buffer because we want to make sure every player input gets executed
    // for at least one tick, so things like firing are correctly handled.
    // This should not cause the buffer to pile up since the networking system is run many times per tick.
    if (conn->read(buffer)) {
      BOOST_LOG_TRIVIAL(debug) << "Received message from client (" << static_cast<unsigned int>(client)
                               << "): " << buffer << std::endl;
      auto buff_ptr = reinterpret_cast<const uint8_t *>(buffer.c_str());
      auto verifier = flatbuffers::Verifier(buff_ptr, buffer.length());
      bool ok = ::spac::net::VerifyPacketBuffer(verifier);
      if (!ok) {
        BOOST_LOG_TRIVIAL(debug) << "Received malformed packet." << std::endl;
        continue;
      }
      auto packet = ::spac::net::GetPacket(buff_ptr);
      handlePacket(client, packet);
      // handle the buffer message
    }

    if (conn->error()) {
      // network connection is in error state, tag the client for destruction
      auto err = conn->close();
      BOOST_LOG_TRIVIAL(debug) << "Closing connection to client due to error: " << err.message() << std::endl;
      registry_.assign<component::TaggedToDestroy>(client);
    }
  }

  // todo: might need a "dead" component to simplify things
  // Send death messages
  for (auto client : deathObserver_) {
    auto &net = registry_.get<component::SessionComponent>(client);
    if (net.spawned.time_since_epoch().count() > 0) {  // make sure the client didn't just connect
      auto timeAlive = duration_cast<milliseconds>(now - net.spawned);

      flatbuffers::FlatBufferBuilder builder(64);  // todo set to exact size
      auto death = ::spac::net::CreateDeath(builder, timeAlive.count());
      auto packet = ::spac::net::CreatePacket(builder, ::spac::net::Message_Death, death.Union());
      ::spac::net::FinishPacketBuffer(builder, packet);
      std::string buffer(reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
      if (auto err = net.conn->write(buffer); err) {
        registry_.destroy(client);
      }
    }
  }
  deathObserver_.clear();
}

void NetworkingSystem::handleRespawn(entt::entity entity, const ::spac::net::Packet *packet) {
  auto name = packet->message_as_Respawn()->name()->str();
  if (!registry_.has<component::Named>(entity)) {
    // spawn player
    // todo choose spawn position more intelligently
    auto spawnPosition = b2Vec2_zero;

    registry_.assign<component::Named>(entity, name);
    registry_.assign<component::Owner>(entity);
    registry_.assign<component::ShipController>(entity);
    registry_.assign<component::Health>(entity, 100.0f);
    // todo create fuel fixture
    registry_.assign<component::Fuel>(entity);
    registry_.assign<component::Booster>(entity);
    registry_.assign<component::Shielded>(entity);
    registry_.assign<component::SideBooster>(entity);
    registry_.assign<component::DealsDamage>(entity, 0.1f);
    registry_.assign<component::Perceivable>(entity, component::Perceivable::Kind::SHIP);
    registry_.assign<component::Sensing>(entity);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(spawnPosition.x, spawnPosition.y);
    bodyDef.allowSleep = true;
    bodyDef.awake = true;
    auto *id = new entt::entity(entity);
    bodyDef.userData = reinterpret_cast<void *>(id);
    b2Body *body = world_.CreateBody(&bodyDef);

    // todo add shield sensor fixture

    b2CircleShape sensorShape;
    sensorShape.m_radius = PERCEPTION_RADIUS;

    b2FixtureDef sensorFixtureDef;
    sensorFixtureDef.isSensor = true;
    sensorFixtureDef.userData = reinterpret_cast<void *>(id);
    sensorFixtureDef.filter.maskBits = CollisionMask::HEALTH | CollisionMask::DAMAGER;
    sensorFixtureDef.filter.categoryBits = CollisionMask::SENSOR;

    b2Vec2 vertices[3];
    vertices[0].Set(0.0f, 5.0f - 5.0f / 3.0f);
    vertices[1].Set(-2.0f, -5.0f / 3.0f);
    vertices[2].Set(2.0f, -5.0f / 3.0f);

    int32_t count = 3;
    b2PolygonShape polygonShape;
    polygonShape.Set(vertices, count);

    b2FixtureDef fixtureDef;
    fixtureDef.userData = reinterpret_cast<void *>(id);
    fixtureDef.filter.maskBits = CollisionMask::HEALTH | CollisionMask::DAMAGER | CollisionMask::SENSOR;
    fixtureDef.filter.categoryBits = CollisionMask::HEALTH | CollisionMask::DAMAGER;
    fixtureDef.shape = &polygonShape;
    fixtureDef.density = SHIP_DENSITY;
    fixtureDef.restitution = 0.8f;  // todo tune restitution
    registry_.assign<component::PhysicsBody>(entity, body);
  }
}

void NetworkingSystem::listen() { do_accept(); }

void NetworkingSystem::do_accept() {
  BOOST_LOG_TRIVIAL(debug) << "do_accept" << std::endl;
  acceptor_.async_accept(asio::make_strand(ioc_),
                         beast::bind_front_handler(&NetworkingSystem::on_accept, shared_from_this()));
}
void NetworkingSystem::on_accept(beast::error_code ec, tcp::socket socket) {
  BOOST_LOG_TRIVIAL(debug) << "on_accept" << std::endl;
  if (ec) {
    fail(ec, "accept");
  } else {
    auto session = net::WebsocketSession::create(std::move(socket), ctx_);
    session->run();
    registry_.assign<component::SessionComponent>(registry_.create(), session);
  }
  do_accept();
}

void NetworkingSystem::handlePacket(entt::entity entity, const ::spac::net::Packet *packet) {}

}  // namespace spac::server::system