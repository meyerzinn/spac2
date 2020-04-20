#include "BoosterSystem.h"
#include <PhysicsComponents.h>
#include <ShipComponents.h>
#include <Constants.h>

namespace spac::server::system {
    void BoosterSystem::update() {
        auto entities = mRegistry.view<component::ShipController, component::PhysicsBody, component::Fuel, component::Booster>();
        for (auto entity : entities) {
            auto controller = entities.get<component::ShipController>(entity);
            auto &booster = entities.get<component::Booster>(entity);
            auto physics = entities.get<component::PhysicsBody>(entity);
            auto &fuel = entities.get<component::Fuel>(entity);
            if (controller.booster <= 0 || fuel.stored <= 0 ||
                physics.body->GetLinearVelocity().Length() > booster.maxLinearVelocity) {
                booster.lastBurnedMass = 0;
                continue;
            }
            auto exhaustMass = std::clamp(controller.booster * booster.burnRate * FIXED_SIMULATION_DURATION, 0.f,
                                          fuel.stored);
            auto force = booster.exhaustSpeed * exhaustMass / FIXED_SIMULATION_DURATION;
            physics.body->ApplyForceToCenter(b2Vec2(0, force), true);
            fuel.stored -= exhaustMass;
            booster.lastBurnedMass = exhaustMass;
        }
    }
}
