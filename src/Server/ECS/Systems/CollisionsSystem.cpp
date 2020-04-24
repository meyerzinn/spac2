#include "CollisionsSystem.h"
#include "CollisionFlags.h"
#include "Constants.h"
#include "EntityComponents.h"
constexpr int32_t velocityIterations = 8;
constexpr int32_t positionIterations = 3;

namespace spac::server::system {
void CollisionsSystem::update() { mWorld.Step(FIXED_SIMULATION_DURATION, velocityIterations, positionIterations); }

CollisionsSystem::CollisionsSystem(entt::registry &registry, b2World &world) : System(registry), mWorld(world) {
  world.SetContactListener(this);
}

void CollisionsSystem::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
  //  auto entityA = *reinterpret_cast<entt::entity *>(contact->GetFixtureA()->GetBody()->GetUserData());
  //  auto entityB = *reinterpret_cast<entt::entity *>(contact->GetFixtureB()->GetBody()->GetUserData());
  auto normalImpulseMagnitude = sqrt(impulse->normalImpulses[0] * impulse->normalImpulses[0] +
                                     impulse->normalImpulses[1] * impulse->normalImpulses[1]);
  auto entityA = *reinterpret_cast<entt::entity *>(contact->GetFixtureA()->GetBody()->GetUserData());
  auto entityB = *reinterpret_cast<entt::entity *>(contact->GetFixtureB()->GetBody()->GetUserData());
  this->handleCollision(entityA, entityB, normalImpulseMagnitude);
  this->handleCollision(entityB, entityA, normalImpulseMagnitude);
}

void CollisionsSystem::handleCollision(entt::entity entityA, entt::entity entityB, float impulse) {
  if (auto dealsDamageComponent = mRegistry.try_get<component::DealsDamage>(entityA); dealsDamageComponent) {
    if (auto healthComponent = mRegistry.try_get<component::Health>(entityB); healthComponent) {
      auto damage = impulse * dealsDamageComponent->scalar;
      healthComponent->current -= damage;
    }
  }
}

}  // namespace spac::server::system
