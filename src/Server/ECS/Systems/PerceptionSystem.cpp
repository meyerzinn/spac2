#include "PerceptionSystem.h"

#include <ECS/Components/EntityComponents.h>
#include <ECS/Components/PerceptionComponents.h>
#include <ECS/Components/PhysicsComponents.h>
#include <ECS/Components/Session.h>
#include <ECS/Components/ShipComponents.h>
#include <packet_generated.h>
#include <Utilities/CollisionFlags.h>
#include <Utilities/Constants.h>
#include <flatbuffers/flatbuffers.h>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <unordered_set>
#include <vector>

using namespace std::chrono;

namespace spac::server::net {
::spac::net::vec2f b2Vec2_to_vec2f(b2Vec2 v) { return ::spac::net::vec2f(v.x, v.y); }
}  // namespace spac::server::net

namespace spac::server::system {

constexpr milliseconds PERCEPTION_PERIOD(50);
constexpr float PERCEPTION_RADIUS = 1000;  // in meters

PerceptionSystem::PerceptionSystem(entt::registry &registry) : System(registry) {}

void PerceptionSystem::update() {
  auto now = system_clock::now();
  // send perceptions
  if (now - lastPerception_ > PERCEPTION_PERIOD) {
    lastPerception_ = now;

    // some top-level views to optimize random access patterns
    auto ships = registry_.view<component::PhysicsBody, component::Health, component::Booster, component::SideBooster,
                                component::Shielded>();
    auto projectiles =
        registry_.view<component::PhysicsBody, component::Perceivable, component::DealsDamage, component::Health>(
            entt::exclude<component::SessionComponent>);

    auto clients = registry_.view<component::SessionComponent, component::Sensing>();
    for (auto client : clients) {
      // use a separate builder for each client to avoid needing to copy memory
      flatbuffers::FlatBufferBuilder builder(1024);

      auto &session = clients.get<component::SessionComponent>(client);
      auto sensing = clients.get<component::Sensing>(client);

      std::unordered_set<entt::entity> knownEntities;

      std::vector<flatbuffers::Offset<::spac::net::ShipMetadata>> shipMetas;
      std::vector<flatbuffers::Offset<::spac::net::ShipDelta>> shipDeltas;
      std::vector<flatbuffers::Offset<::spac::net::ProjectileMetadata>> projectileMetas;
      std::vector<flatbuffers::Offset<::spac::net::ProjectileDelta>> projectileDeltas;

      for (auto entity : sensing.entities) {
        auto perceivable = registry_.get<component::Perceivable>(entity);
        switch (perceivable.kind) {
          case component::Perceivable::Kind::SHIP: {
            knownEntities.insert(entity);
            auto [oPhysics, oHealth, oBooster, oSideBooster, oShield] =
                registry_.get<component::PhysicsBody, component::Health, component::Booster, component::SideBooster,
                              component::Shielded>(entity);
            if (session.known.find(entity) == session.known.end()) {
              // introduce the ship
              auto oShip = registry_.get<component::Named>(entity);
              auto shipMeta =
                  ::spac::net::CreateShipMetadataDirect(builder, static_cast<uint32_t>(entity), oShip.name.c_str());
              shipMetas.push_back(shipMeta);
              session.known.insert(entity);
            }

            auto health = uint8_t(
                std::min<float>(256 * std::ceil(std::clamp<float>(oHealth.current / oHealth.max, 0.f, 1.f)), 255));

            uint8_t flags = 0;

            auto boosterMassEjectedProportion = std::min<std::uint8_t>(
                7, std::uint8_t(8.0f * std::clamp<float>(oBooster.lastBurnedMass /
                                                             (oBooster.exhaustSpeed * FIXED_SIMULATION_DURATION),
                                                         0.0f, 1.0f)));
            flags |= uint8_t(boosterMassEjectedProportion);

            auto sideBoosterMassEjectedProportion =
                std::min<float>(3, 4 * std::clamp<float>(std::abs(oSideBooster.lastBurnedMass) /
                                                             (oSideBooster.exhaustSpeed * FIXED_SIMULATION_DURATION),
                                                         0, 1));
            flags |= (uint8_t(sideBoosterMassEjectedProportion) << 4u);
            if (oSideBooster.lastBurnedMass >= 0) flags |= (1u << 3u);

            if (oShield.fixture) flags |= (1u << 7u);

            auto position = net::b2Vec2_to_vec2f(oPhysics.body->GetPosition());
            auto velocity = net::b2Vec2_to_vec2f(oPhysics.body->GetLinearVelocity());
            auto shipDelta = ::spac::net::CreateShipDelta(builder, static_cast<uint32_t>(entity), &position, &velocity,
                                                          oPhysics.body->GetAngle(),
                                                          oPhysics.body->GetAngularVelocity(), health, flags);
            shipDeltas.push_back(shipDelta);
            break;
          }
          case component::Perceivable::Kind::PROJECTILE: {
            knownEntities.insert(entity);
            auto oPhysics = registry_.get<component::PhysicsBody>(entity);
            if (session.known.find(entity) == session.known.end()) {
              auto owned = registry_.get<component::Owned>(entity);
              auto projectileMeta = ::spac::net::CreateProjectileMetadata(builder, static_cast<uint32_t>(entity),
                                                                          static_cast<uint32_t>(owned.owner));
              projectileMetas.push_back(projectileMeta);
            }
            auto position = net::b2Vec2_to_vec2f(oPhysics.body->GetPosition());
            auto velocity = net::b2Vec2_to_vec2f(oPhysics.body->GetLinearVelocity());

            auto projectileDelta =
                ::spac::net::CreateProjectileDelta(builder, static_cast<uint32_t>(entity), &position, &velocity);
            projectileDeltas.push_back(projectileDelta);
            break;
          }
          default:
            // how would this even happen?
            BOOST_LOG_TRIVIAL(debug) << "Encountered entity for which no serialization strategy is known." << std::endl;
            break;
        }
      }

      auto fuel = registry_.get<component::Fuel>(client);
      auto fuelProportion = std::min<float>(std::ceil(256 * std::clamp<float>(fuel.stored / fuel.capacity, 0, 1)), 255);
      auto fuelEncoded = uint8_t(fuelProportion);

      std::vector<entt::entity> removed;
      std::set_difference(session.known.begin(), session.known.end(), knownEntities.begin(), knownEntities.end(),
                          std::back_inserter(removed));
      session.known = boost::container::flat_set<entt::entity>(removed.begin(), removed.end());

      uint64_t timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch())
                               .count();  // use a different timestamp for each connection since
      // we want to find the lag due to network,
      // not the server time preparing perceptions

      auto perception = ::spac::net::CreatePerceptionDirect(
          builder, timestamp, fuelEncoded, reinterpret_cast<std::vector<unsigned int> *>(&removed), &shipMetas,
          &projectileMetas, &shipDeltas, &projectileDeltas);
      auto packet = ::spac::net::CreatePacket(builder, ::spac::net::Message_Perception, perception.Union());
      ::spac::net::FinishPacketBuffer(builder, packet);

      std::string buffer(reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
      session.conn->write(buffer);
    }
  }
}
}  // namespace spac::server::system