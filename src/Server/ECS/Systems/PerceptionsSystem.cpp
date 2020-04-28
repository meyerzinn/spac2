#include "PerceptionsSystem.h"
#include <CollisionFlags.h>
#include <flatbuffers/flatbuffers.h>
#include <chrono>
#include <vector>
#include "Constants.h"
#include "EntityComponents.h"
#include "NetworkingComponents.h"
#include "PhysicsComponents.h"
#include "ShipComponents.h"
#include "packet_generated.h"
using namespace std::chrono;

namespace spac::net {
vec2f b2Vec2_to_vec2f(b2Vec2 v) { return vec2f(v.x, v.y); }
}  // namespace spac::net

namespace spac::server::system {

constexpr milliseconds PERCEPTION_PERIOD(50);
constexpr float PERCEPTION_RADIUS = 1000;  // in meters

template <bool SSL>
PerceptionsSystem<SSL>::PerceptionsSystem(entt::registry &registry, /*b2World &world,*/ uWS::Loop *loop)
    : System(registry), /*mWorld(world),*/ mLoop(loop) {}

template <bool SSL>
void PerceptionsSystem<SSL>::update() {
  auto now = high_resolution_clock::now();
  // send perceptions
  if (now - mLastPerception > PERCEPTION_PERIOD) {
    mLastPerception = now;

    // some top-level views to optimize random access patterns
    auto ships = mRegistry.view<component::PhysicsBody, component::Health, component::Booster, component::SideBooster,
                                component::Shielded>();
    auto projectiles =
        mRegistry.view<component::PhysicsBody, component::Perceivable, component::DealsDamage, component::Health>(
            entt::exclude<component::NetClient<true>>);

    auto clients = mRegistry.view<component::NetClient<SSL>, component::Sensing>();
    for (auto client : clients) {
      // use a separate builder for each client to avoid needing to copy memory
      flatbuffers::FlatBufferBuilder builder(1024);

      auto &net = clients.template get<component::NetClient<SSL>>(client);
      auto sensing = clients.template get<component::Sensing>(client);

      std::unordered_set<entt::entity> knownEntities;

      std::vector<flatbuffers::Offset<net::ShipMetadata>> shipMetas;
      std::vector<flatbuffers::Offset<net::ShipDelta>> shipDeltas;
      std::vector<flatbuffers::Offset<net::ProjectileMetadata>> projectileMetas;
      std::vector<flatbuffers::Offset<net::ProjectileDelta>> projectileDeltas;

      for (auto entity : sensing.entities) {
        auto perceivable = mRegistry.get<component::Perceivable>(entity);
        switch (perceivable.kind) {
          case component::Perceivable::Kind::SHIP: {
            knownEntities.insert(entity);
            auto [oPhysics, oHealth, oBooster, oSideBooster, oShield] =
                mRegistry.get<component::PhysicsBody, component::Health, component::Booster, component::SideBooster,
                              component::Shielded>(entity);
            if (net.knownEntities.find(entity) == net.knownEntities.end()) {
              // introduce the ship
              auto oShip = mRegistry.get<component::Named>(entity);
              auto shipMeta = net::CreateShipMetadataDirect(builder, static_cast<uint32_t>(entity), oShip.name.c_str());
              shipMetas.push_back(shipMeta);
              net.knownEntities.insert(entity);
            }

            auto health = uint8_t(
                std::min<float>(256 * std::ceil(std::clamp<float>(oHealth.current / oHealth.max, 0., 1.)), 255));

            uint8_t flags;

            auto boosterMassEjectedProportion = std::min<float>(
                7, 8.0 * std::clamp<float>(
                             oBooster.lastBurnedMass / (oBooster.exhaustSpeed * FIXED_SIMULATION_DURATION), 0., 1.));
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
            auto shipDelta =
                net::CreateShipDelta(builder, static_cast<uint32_t>(entity), &position, &velocity,
                                     oPhysics.body->GetAngle(), oPhysics.body->GetAngularVelocity(), health, flags);
            shipDeltas.push_back(shipDelta);
            break;
          }
          case component::Perceivable::Kind::PROJECTILE: {
            knownEntities.insert(entity);
            auto oPhysics = mRegistry.get<component::PhysicsBody>(entity);
            if (net.knownEntities.find(entity) == net.knownEntities.end()) {
              auto owned = mRegistry.get<component::Owned>(entity);
              auto projectileMeta = net::CreateProjectileMetadata(builder, static_cast<uint32_t>(entity),
                                                                  static_cast<uint32_t>(owned.owner));
              projectileMetas.push_back(projectileMeta);
            }
            auto position = net::b2Vec2_to_vec2f(oPhysics.body->GetPosition());
            auto velocity = net::b2Vec2_to_vec2f(oPhysics.body->GetLinearVelocity());

            auto projectileDelta =
                net::CreateProjectileDelta(builder, static_cast<uint32_t>(entity), &position, &velocity);
            projectileDeltas.push_back(projectileDelta);
            break;
          }
          default:
            // how would this even happen?
            break;
        }
      }

      auto fuel = mRegistry.get<component::Fuel>(client);
      auto fuelProportion = std::min<float>(std::ceil(256 * std::clamp<float>(fuel.stored / fuel.capacity, 0, 1)), 255);
      auto fuelEncoded = uint8_t(fuelProportion);

      std::vector<entt::entity> removed;
      std::set_difference(net.knownEntities.begin(), net.knownEntities.end(), knownEntities.begin(),
                          knownEntities.end(), std::back_inserter(removed));
      net.knownEntities = std::unordered_set<entt::entity>(removed.begin(), removed.end());

      uint64_t timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch())
                               .count();  // use a different timestamp for each connection since
      // we want to find the lag due to network,
      // not the server time preparing perceptions

      auto perception = net::CreatePerceptionDirect(builder, timestamp, fuelEncoded,
                                                    reinterpret_cast<std::vector<unsigned int> *>(&removed), &shipMetas,
                                                    &projectileMetas, &shipDeltas, &projectileDeltas);
      auto packet = net::CreatePacket(builder, net::Message_Perception, perception.Union());
      net::FinishPacketBuffer(builder, packet);

      std::string_view buffer(reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
      // we have to send from the main thread
      mLoop->defer([&net, buffer]() { net.conn->send(buffer); });
    }
  }
}

template class PerceptionsSystem<true>;
template class PerceptionsSystem<false>;
}  // namespace spac::server::system