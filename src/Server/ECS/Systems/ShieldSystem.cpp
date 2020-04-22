#include "ShieldSystem.h"
#include "Constants.h"
#include "PhysicsComponents.h"
#include "ShipComponents.h"
namespace spac::server::system {

constexpr float SHIELD_RADIUS = 3.0f;

void ShieldSystem::update() {
  auto entities =
      mRegistry.view<component::PhysicsBody, component::Fuel, component::ShipController, component::Shield>();
  for (auto entity : entities) {
    auto &shield = entities.get<component::Shield>(entity);
    auto controller = entities.get<component::ShipController>(entity);
    auto physics = entities.get<component::PhysicsBody>(entity);
    auto fuelRequired = shield.burnRate * FIXED_SIMULATION_DURATION;
    auto fuel = entities.get<component::Fuel>(entity);
    if (fuel.stored < fuelRequired) {
      // out of fuel, disable the shield
      disableShield(shield, physics);
      continue;
    }
    if (controller.shield) {
      fuel.stored -= fuelRequired;
      enableShield(shield, physics);
    } else {
      if (shield.fixture) {
        // disable shield
      }
    }
    // disengage shield
  }
}

void ShieldSystem::enableShield(component::Shield &shield, component::PhysicsBody physics) {
  if (shield.fixture) return;  // shield already enabled
  auto body = physics.body;
  b2CircleShape shape;
  shape.m_radius = SHIELD_RADIUS;
}

void ShieldSystem::disableShield(component::Shield &shield, component::PhysicsBody physics) {
  if (!shield.fixture) return;  // shield already disabled
  auto body = physics.body;
  body->DestroyFixture(shield.fixture);
  shield.fixture = nullptr;
}
ShieldSystem::ShieldSystem(entt::registry &registry) : System(registry) { shieldShape.m_radius = SHIELD_RADIUS; }

}  // namespace spac::server::system