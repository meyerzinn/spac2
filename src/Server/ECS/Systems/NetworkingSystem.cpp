#include "Constants.h"
#include "NetworkingSystem.h"
#include "NetworkingComponents.h"
#include "PhysicsComponents.h"
#include "EntityComponents.h"
#include "ShipComponents.h"
#include "packet_generated.h"

#include <flatbuffers/flatbuffers.h>
#include <algorithm>
#include <unordered_set>
#include <chrono>

namespace spac::net {
    vec2f b2Vec2_to_vec2f(b2Vec2 v) {
        return vec2f(v.x, v.y);
    }
}

using namespace std::chrono;

namespace spac::server::system {
    template<bool SSL>
    NetworkingSystem<SSL>::NetworkingSystem(b2World *world): mWorld(world) {}

    template<bool SSL>
    void NetworkingSystem<SSL>::update() {
        uint64_t ms = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
        ).count();

        auto clients = mRegistry.view<component::PhysicsBody, component::NetClient<SSL>>();
        for (auto client : clients) {
            auto net = mRegistry.get<component::NetClient<SSL>>(client);
            component::PhysicsBody physics = mRegistry.get<component::PhysicsBody>(client);

            NetQueryCallback queryCallback;
            b2AABB viewableRegion; // TODO make a separate, smaller region for sending metadata
            auto center = physics.body->GetWorldCenter();
            viewableRegion.lowerBound = center + b2Vec2(-PERCEPTION_RADIUS, -PERCEPTION_RADIUS);
            viewableRegion.upperBound = center + b2Vec2(PERCEPTION_RADIUS, PERCEPTION_RADIUS);
            mWorld->QueryAABB(&queryCallback, viewableRegion);

            flatbuffers::FlatBufferBuilder builder(1024);

            std::vector<flatbuffers::Offset<net::ShipMetadata>> shipMetas;
            std::vector<flatbuffers::Offset<net::ShipDelta>> shipDeltas;

            std::unordered_set<entt::entity> knownEntities;
            for (auto shipId : queryCallback.foundShips) {
                knownEntities.insert(shipId);
                auto[oPhysics, oHealth, oBooster, oSideBooster, oShield] = mRegistry.get<component::PhysicsBody, component::Health, component::Booster, component::SideBooster, component::Shield>(
                        shipId);
                if (net.knownEntities.find(shipId) == net.knownEntities.end()) {
                    // introduce the ship
                    auto oShip = mRegistry.get<component::Named>(shipId);
                    auto shipMeta = net::CreateShipMetadataDirect(builder, static_cast<uint32_t>(shipId), oShip.name);
                    shipMetas.push_back(shipMeta);
                    net.knownEntities.insert(shipId);
                }

                auto health = uint8_t(std::min(256 * std::ceil(std::clamp(oHealth.current / oHealth.max, 0, 1)), 255));

                uint8_t flags;

                auto boosterMassEjectedProportion = std::min(7, 8.0 * std::clamp(
                        oBooster.lastBurnedMass / (oBooster.exhaustSpeed * FIXED_SIMULATION_DURATION), 0, 1));
                flags |= uint8_t(boosterMassEjectedProportion);

                auto sideBoosterMassEjectedProportion = std::min(3, 4 * std::clamp(
                        std::abs(oSideBooster.lastBurnedMass) / (oSideBooster.exhaustSpeed * FIXED_SIMULATION_DURATION),
                        0, 1));
                flags |= (uint8_t(sideBoosterMassEjectedProportion) << 4u);
                if (oSideBooster.lastBurnedMass >= 0) flags |= (1u << 3u);

                if (oShield.engaged) flags |= (1u << 7u);

                auto position = net::b2Vec2_to_vec2f(oPhysics.body->GetPosition());
                auto velocity = net::b2Vec2_to_vec2f(oPhysics.body->GetLinearVelocity());
                auto shipDelta = net::CreateShipDelta(builder, static_cast<uint32_t>(shipId),
                                                      &position,
                                                      &velocity,
                                                      physics.body->GetAngle(),
                                                      physics.body->GetAngularVelocity(),
                                                      health,
                                                      flags);
                shipDeltas.push_back(shipDelta);
            }

            std::vector<flatbuffers::Offset<net::ProjectileMetadata>> projectileMetas;
            std::vector<flatbuffers::Offset<net::ProjectileDelta>> projectileDeltas;

            for (auto projectileId: queryCallback.foundProjectiles) {
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

                auto projectileDelta = net::CreateProjectileDelta(builder,
                                                                  static_cast<uint32_t>(projectileId),
                                                                  &position,
                                                                  &velocity);
                projectileDeltas.push_back(projectileDelta);
            }

            auto fuel = mRegistry.get<component::Fuel>(client);
            auto fuelProportion = std::min(std::ceil(256 * std::clamp(fuel.stored / fuel.capacity, 0, 1)), 255);
            auto fuelEncoded = uint8_t(fuelProportion);

            std::vector<entt::entity> removed;
            std::set_difference(net.knownEntities.begin(), net.knownEntities.end(),
                                knownEntities.begin(), knownEntities.end(),
                                std::back_inserter(removed));
            net.knownEntities = std::unordered_set(removed.begin(), removed.end());
            auto perception = net::CreatePerceptionDirect(builder, ms, 0,
                                                          reinterpret_cast<std::vector<unsigned int> *>(&removed),
                                                          &shipMetas, &projectileMetas, &shipDeltas,
                                                          &projectileDeltas);
            auto packet = net::CreatePacket(builder, net::Message_Perception, perception.Union());
            net::FinishPacketBuffer(builder, packet);
            net.conn->send(std::basic_string_view(builder.GetBufferPointer(), builder.GetSize()), uWS::OpCode::BINARY);

//            auto[oPhysics, oHealth, oBooster, oShield] = mRegistry.get<component::PhysicsBody, component::Health, component::Booster, component::Shield>(
//                    queryCallback.foundShips);


        }
    }

    template<bool SSL>
    bool NetworkingSystem<SSL>::NetQueryCallback::ReportFixture(b2Fixture *fixture) {
        if ((fixture->GetFilterData().categoryBits & EntityCategory::SHIP) != 0u) {
            foundShips.insert(*static_cast<entt::entity *>(fixture->GetBody()->GetUserData()));
        } else if ((fixture->GetFilterData().categoryBits & EntityCategory::PROJECTILE) != 0u) {
            foundProjectiles.insert(*static_cast<entt::entity *>(fixture->GetBody()->GetUserData()));
        }
        return true;
    };
}