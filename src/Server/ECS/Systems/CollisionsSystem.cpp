#include "CollisionsSystem.h"
#include "EntityComponents.h"
#include <Constants.h>

constexpr int32_t velocityIterations = 8;
constexpr int32_t positionIterations = 3;

namespace spac::server::system {
    void CollisionsSystem::update() {
        mWorld.Step(FIXED_SIMULATION_DURATION, velocityIterations, positionIterations);
    }

    CollisionsSystem::CollisionsSystem(entt::registry &registry, b2World &world) : System(registry), mWorld(world) {
        world.SetContactListener(this);
    }

    void CollisionsSystem::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
        auto entityA = *static_cast<entt::entity *>(contact->GetFixtureA()->GetUserData());
        auto entityB = *static_cast<entt::entity *>(contact->GetFixtureB()->GetUserData());
        auto normalImpulseMagnitude = sqrt(impulse->normalImpulses[0] * impulse->normalImpulses[0] +
                                           impulse->normalImpulses[1] * impulse->normalImpulses[1]);
        this->handleCollision(entityA, entityB, normalImpulseMagnitude);
        this->handleCollision(entityB, entityA, normalImpulseMagnitude);
    }

    void CollisionsSystem::handleCollision(entt::entity damager, entt::entity target, float impulse) {
        auto dealsDamageComponent = mRegistry.try_get<component::DealsDamage>(damager);
        if (dealsDamageComponent == nullptr) return;
        auto healthComponent = mRegistry.try_get<component::Health>(target);
        if (healthComponent == nullptr) return;
        auto damage = (int) floor(impulse * dealsDamageComponent->scalar);
        healthComponent->current -= damage;
    }

//    void CollisionsSystem::BeginContact(b2Contact *contact) {
//        auto entityA = (entt::entity *) contact->GetFixtureA()->GetUserData();
//        auto entityB = (entt::entity *) contact->GetFixtureB()->GetUserData();
//
//    }

//    void CollisionsSystem::EndContact(b2Contact *contact) {
//
//    }

}
