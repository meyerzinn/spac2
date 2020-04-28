#include "CollisionsSystem.h"
#include "CollisionFlags.h"
#include "Constants.h"
#include "EntityComponents.h"
#include "PhysicsComponents.h"

#include <NetworkingComponents.h>

constexpr int32_t velocityIterations = 8;
constexpr int32_t positionIterations = 3;

namespace spac::server::system {
void CollisionsSystem::update() { mWorld.Step(FIXED_SIMULATION_DURATION, velocityIterations, positionIterations); }

CollisionsSystem::CollisionsSystem(entt::registry &registry, b2World &world) : System(registry), mWorld(world) {
  world.SetContactListener(this);
}

void CollisionsSystem::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
  auto normalImpulseMagnitude = sqrt(impulse->normalImpulses[0] * impulse->normalImpulses[0] +
                                     impulse->normalImpulses[1] * impulse->normalImpulses[1]);
  auto entityA = *reinterpret_cast<entt::entity *>(contact->GetFixtureA()->GetUserData());
  auto entityB = *reinterpret_cast<entt::entity *>(contact->GetFixtureB()->GetUserData());
  this->handleImpulsiveCollision(entityA, entityB, normalImpulseMagnitude);
  this->handleImpulsiveCollision(entityB, entityA, normalImpulseMagnitude);
}

void CollisionsSystem::BeginContact(b2Contact *contact) {
  if ((contact->GetFixtureA()->GetFilterData().maskBits & contact->GetFixtureB()->GetFilterData().maskBits &
       CollisionMask::SENSOR) > 0) {
    auto entityA = *reinterpret_cast<entt::entity *>(contact->GetFixtureA()->GetUserData());
    auto entityB = *reinterpret_cast<entt::entity *>(contact->GetFixtureB()->GetUserData());

    this->handleBeginSensorCollision(entityA, entityB);
    this->handleBeginSensorCollision(entityB, entityA);
  }
}

void CollisionsSystem::EndContact(b2Contact *contact) {
  if ((contact->GetFixtureA()->GetFilterData().maskBits & contact->GetFixtureB()->GetFilterData().maskBits &
       CollisionMask::SENSOR) > 0) {
    auto entityA = *reinterpret_cast<entt::entity *>(contact->GetFixtureA()->GetUserData());
    auto entityB = *reinterpret_cast<entt::entity *>(contact->GetFixtureB()->GetUserData());

    this->handleEndSensorCollision(entityA, entityB);
    this->handleEndSensorCollision(entityB, entityA);
  }
}

void CollisionsSystem::handleImpulsiveCollision(entt::entity entityA, entt::entity entityB, float impulse) {
  if (auto dealsDamageComponent = mRegistry.try_get<component::DealsDamage>(entityA); dealsDamageComponent) {
    if (auto healthComponent = mRegistry.try_get<component::Health>(entityB); healthComponent) {
      auto damage = impulse * dealsDamageComponent->scalar;
      healthComponent->current -= damage;
    }
  }
}

void CollisionsSystem::handleBeginSensorCollision(entt::entity entityA, entt::entity entityB) {
  if (auto sensor = mRegistry.try_get<component::Sensing>(entityA); sensor) {
    if (mRegistry.has<component::Perceivable>(entityB)) sensor->entities.insert(entityB);
  }
}

void CollisionsSystem::handleEndSensorCollision(entt::entity entityA, entt::entity entityB) {
  if (auto sensor = mRegistry.try_get<component::Sensing>(entityA); sensor) {
    sensor->entities.erase(entityB);
  }
}

}  // namespace spac::server::system
