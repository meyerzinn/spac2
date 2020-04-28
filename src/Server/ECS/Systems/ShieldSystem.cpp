#include "ShieldSystem.h"
#include "Constants.h"
#include "EntityComponents.h"
#include "PhysicsComponents.h"
#include "ShipComponents.h"
namespace spac::server::system {

constexpr float SHIELD_RADIUS = 3.0f;

void ShieldSystem::update() {
  auto entities =
      mRegistry.view<component::PhysicsBody, component::Fuel, component::ShipController, component::Shielded>();
  for (auto entity : entities) {
    auto &shielded = entities.get<component::Shielded>(entity);
    auto controller = entities.get<component::ShipController>(entity);
    auto physics = entities.get<component::PhysicsBody>(entity);
    auto fuelRequired = shielded.burnRate * FIXED_SIMULATION_DURATION;
    auto &fuel = entities.get<component::Fuel>(entity);
    if (fuel.stored < fuelRequired) {
      // out of fuel, disable the shield
      disableShield(shielded);
      continue;
    }
    if (controller.shield) {
      fuel.stored -= fuelRequired;
      enableShield(entity, shielded);
    } else {
      disableShield(shielded);
    }
    // disengage shield
  }
}

void ShieldSystem::enableShield(entt::entity entity, component::Shielded &shielded) {
  if (shielded.shield != entt::null) return;
  auto shield = mRegistry.create();
  shielded.shield = shield;
  mRegistry.assign<component::Sensing>(shield);
  mRegistry.assign<component::Owned>(shield, entity);
  mRegistry.get<component::Owner>(entity).owned.push_back(shield);
}

void ShieldSystem::disableShield(component::Shielded &shielded) {
  if (shielded.shield == entt::null) return;
  mRegistry.assign<component::TaggedToDestroy>(shielded.shield);
  shielded.shield = entt::null;
  //  if (!shield.fixture) return;  // shield already disabled
  //  auto body = physics.body;
  //  body->DestroyFixture(shield.fixture);
  //  shield.fixture = nullptr;
}

}  // namespace spac::server::system