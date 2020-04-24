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
PerceptionsSystem<SSL>::PerceptionsSystem(entt::registry &registry, b2World &world, uWS::Loop *loop)
    : System(registry), mWorld(world), mLoop(loop) {}

template <bool SSL>
void PerceptionsSystem<SSL>::update() {
  auto now = high_resolution_clock::now();
  // send perceptions
  if (now - mLastPerception > PERCEPTION_PERIOD) {
    mLastPerception = now;

    auto clients = mRegistry.view<component::NetClient<SSL>>();
    for (auto client : clients) {
      // use a separate builder for each client to avoid needing to copy memory
      flatbuffers::FlatBufferBuilder builder(1024);

      auto &net = clients.template get<component::NetClient<SSL>>(client);

      NetQueryCallback queryCallback;
      b2AABB viewableRegion;  // TODO make a separate, smaller region for
      // sending metadata
      auto center = net.lastPosition;
      viewableRegion.lowerBound = center + b2Vec2(-PERCEPTION_RADIUS, -PERCEPTION_RADIUS);
      viewableRegion.upperBound = center + b2Vec2(PERCEPTION_RADIUS, PERCEPTION_RADIUS);
      mWorld.QueryAABB(&queryCallback, viewableRegion);

      std::vector<flatbuffers::Offset<net::ShipMetadata>> shipMetas;
      std::vector<flatbuffers::Offset<net::ShipDelta>> shipDeltas;

      std::unordered_set<entt::entity> knownEntities;
      for (auto shipId : queryCallback.foundShips) {
        knownEntities.insert(shipId);
        auto [oPhysics, oHealth, oBooster, oSideBooster, oShield] =
            mRegistry.get<component::PhysicsBody, component::Health, component::Booster, component::SideBooster,
                          component::Shield>(shipId);
        if (net.knownEntities.find(shipId) == net.knownEntities.end()) {
          // introduce the ship
          auto oShip = mRegistry.get<component::Named>(shipId);
          auto shipMeta = net::CreateShipMetadataDirect(builder, static_cast<uint32_t>(shipId), oShip.name.c_str());
          shipMetas.push_back(shipMeta);
          net.knownEntities.insert(shipId);
        }

        auto health =
            uint8_t(std::min<float>(256 * std::ceil(std::clamp<float>(oHealth.current / oHealth.max, 0., 1.)), 255));

        uint8_t flags;

        auto boosterMassEjectedProportion = std::min<float>(
            7, 8.0 * std::clamp<float>(oBooster.lastBurnedMass / (oBooster.exhaustSpeed * FIXED_SIMULATION_DURATION),
                                       0., 1.));
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
            net::CreateShipDelta(builder, static_cast<uint32_t>(shipId), &position, &velocity,
                                 oPhysics.body->GetAngle(), oPhysics.body->GetAngularVelocity(), health, flags);
        shipDeltas.push_back(shipDelta);
      }

      std::vector<flatbuffers::Offset<net::ProjectileMetadata>> projectileMetas;
      std::vector<flatbuffers::Offset<net::ProjectileDelta>> projectileDeltas;

      for (auto projectileId : queryCallback.foundProjectiles) {
        knownEntities.insert(projectileId);
        auto oPhysics = mRegistry.get<component::PhysicsBody>(projectileId);
        if (net.knownEntities.find(projectileId) == net.knownEntities.end()) {
          auto owned = mRegistry.get<component::Owned>(projectileId);
          auto projectileMeta = net::CreateProjectileMetadata(builder, static_cast<uint32_t>(projectileId),
                                                              static_cast<uint32_t>(owned.owner));
          projectileMetas.push_back(projectileMeta);
        }
        auto position = net::b2Vec2_to_vec2f(oPhysics.body->GetPosition());
        auto velocity = net::b2Vec2_to_vec2f(oPhysics.body->GetLinearVelocity());

        auto projectileDelta =
            net::CreateProjectileDelta(builder, static_cast<uint32_t>(projectileId), &position, &velocity);
        projectileDeltas.push_back(projectileDelta);
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

template <bool SSL>
bool PerceptionsSystem<SSL>::NetQueryCallback::ReportFixture(b2Fixture *fixture) {
  if ((fixture->GetFilterData().categoryBits & EntityCategory::SHIP) != 0u) {
    foundShips.insert(*static_cast<entt::entity *>(fixture->GetBody()->GetUserData()));
  } else if ((fixture->GetFilterData().categoryBits & EntityCategory::PROJECTILE) != 0u) {
    foundProjectiles.insert(*static_cast<entt::entity *>(fixture->GetBody()->GetUserData()));
  }
  return true;
};

template class PerceptionsSystem<true>;
template class PerceptionsSystem<false>;
}  // namespace spac::server::system