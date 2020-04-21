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
#include <cstring>
#include <iostream>

constexpr milliseconds PERCEPTION_PERIOD(50);
constexpr float PERCEPTION_RADIUS = 1000; // in meters

namespace spac::net {
    vec2f b2Vec2_to_vec2f(b2Vec2 v) {
        return vec2f(v.x, v.y);
    }
}

using namespace std::chrono;

namespace spac::server::system {


    template<bool SSL>
    NetworkingSystem<SSL>::NetworkingSystem(entt::registry &registry, b2World &world): System(registry),
                                                                                       mWorld(world) {
        auto const var = &NetworkingSystem<SSL>::onNetClientComponentDestroyed;
        using Component = component::NetClient<SSL>;
        registry.on_destroy<Component>().connect<var>(this);
    }

    template<bool SSL>
    void NetworkingSystem<SSL>::update() {
        QueuedTask *t = nullptr;
        while (mTaskQueue.pop(t)) {
            (*t)();
            delete t;
        }

        auto now = high_resolution_clock::now();
        if (now - lastPerception > PERCEPTION_PERIOD) {
            lastPerception = now;

            flatbuffers::FlatBufferBuilder builder(1024);

            auto clients = mRegistry.view<component::PhysicsBody, component::NetClient<SSL>>();
            for (auto client : clients) {
                builder.Reset();
                auto net = mRegistry.get<component::NetClient<SSL>>(client);
                component::PhysicsBody physics = mRegistry.get<component::PhysicsBody>(client);

                NetQueryCallback queryCallback;
                b2AABB viewableRegion; // TODO make a separate, smaller region for sending metadata
                auto center = physics.body->GetWorldCenter();
                viewableRegion.lowerBound = center + b2Vec2(-PERCEPTION_RADIUS, -PERCEPTION_RADIUS);
                viewableRegion.upperBound = center + b2Vec2(PERCEPTION_RADIUS, PERCEPTION_RADIUS);
                mWorld.QueryAABB(&queryCallback, viewableRegion);


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
                        auto shipMeta = net::CreateShipMetadataDirect(builder, static_cast<uint32_t>(shipId),
                                                                      oShip.name.c_str());
                        shipMetas.push_back(shipMeta);
                        net.knownEntities.insert(shipId);
                    }

                    auto health = uint8_t(
                            std::min<float>(256 * std::ceil(std::clamp<float>(oHealth.current / oHealth.max, 0., 1.)),
                                            255));

                    uint8_t flags;

                    auto boosterMassEjectedProportion = std::min<float>(7, 8.0 * std::clamp<float>(
                            oBooster.lastBurnedMass / (oBooster.exhaustSpeed * FIXED_SIMULATION_DURATION), 0., 1.));
                    flags |= uint8_t(boosterMassEjectedProportion);

                    auto sideBoosterMassEjectedProportion = std::min<float>(3, 4 * std::clamp<float>(
                            std::abs(oSideBooster.lastBurnedMass) /
                            (oSideBooster.exhaustSpeed * FIXED_SIMULATION_DURATION),
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
                        auto projectileMeta = net::CreateProjectileMetadata(builder,
                                                                            static_cast<uint32_t>(projectileId),
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
                auto fuelProportion = std::min<float>(
                        std::ceil(256 * std::clamp<float>(fuel.stored / fuel.capacity, 0, 1)), 255);
                auto fuelEncoded = uint8_t(fuelProportion);

                std::vector<entt::entity> removed;
                std::set_difference(net.knownEntities.begin(), net.knownEntities.end(),
                                    knownEntities.begin(), knownEntities.end(),
                                    std::back_inserter(removed));
                net.knownEntities = std::unordered_set<entt::entity>(removed.begin(), removed.end());

                uint64_t timestamp = duration_cast<milliseconds>(
                        system_clock::now().time_since_epoch()
                ).count(); // use a different timestamp for each connection since we want to find the lag due to network,
                // not the server time preparing perceptions

                auto perception = net::CreatePerceptionDirect(builder, timestamp, fuelEncoded,
                                                              reinterpret_cast<std::vector<unsigned int> *>(&removed),
                                                              &shipMetas, &projectileMetas, &shipDeltas,
                                                              &projectileDeltas);
                auto packet = net::CreatePacket(builder, net::Message_Perception, perception.Union());
                net::FinishPacketBuffer(builder, packet);

                std::string buffer(reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
                net.conn->send(std::string_view(
                        buffer), // todo the copied buffer might be unnecessary if uWebSockets copies into its own buffer anyways
                               uWS::OpCode::BINARY);
            }
        }
    }

    template<bool SSL>
    void NetworkingSystem<SSL>::onNetClientComponentDestroyed(NetworkingSystem<SSL> *instance, entt::registry &_,
                                                              entt::entity entity) {
        // todo notify player of death
    }

    template<bool SSL>
    void NetworkingSystem<SSL>::listen(int port, uWS::TemplatedApp<SSL> app) {
        using SocketData = component::NetClient<SSL>;
        app.get("/hello", [](auto *res, auto *req) {
            res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("Hello HTTP!");
        });

//        .ws<SocketData>("/play", {
//                .compression = uWS::SHARED_COMPRESSOR,
//                .maxPayloadLength = 1024,
//                .idleTimeout = 0,
//                .maxBackpressure = 1 * 1024 * 1024,
//                .open = [](uWS::WebSocket<SSL, true> *ws, uWS::HttpRequest *req) {
//                    std::cout << "connected" << std::endl;
////                    task tsk = [&registry, &ws]() {
////                        auto *data = (socketData *) ws->getUserData();
////                        data->id = registry.create();
////                        registry.assign<WebSocket *>(data->id, ws);
////                    };
////                    while (!task_queue.push(&tsk)) {}
//                },
//                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
//                    std::cout << "message: " << message << std::endl;
//                    ws->send(message, opCode); // this echoes?
//                },
//                .drain = [](auto *ws) {
//                    std::cout << "Drain?" << std::endl;
//                },
//                .ping = [](auto *ws) {
//                    // respond with pong?
//                    std::cout << "Received ping." << std::endl;
//                },
//                .pong = [](auto *ws) {
//                    std::cout << "Received pong." << std::endl;
//                },
//                .close = [](uWS::WebSocket<SSL, true> *ws, int code, std::string_view message) {
//                    std::cout << "Connection closed." << std::endl;
//                }
//        }).listen("0.0.0.0", port, [port](auto *token) {
//            if (!token) {
//                std::cout << "No token!" << std::endl;
//            }
//            std::cout << "Listening on port " << port << std::endl;
//        });
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

    template
    class NetworkingSystem<true>;

    template
    class NetworkingSystem<false>;
}