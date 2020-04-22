#pragma once

#include "PhysicsComponents.h"
#include "ShipComponents.h"
#include "System.h"

namespace spac::server::system {
class ShieldSystem : public spac::System {
 public:
  ShieldSystem(entt::registry &registry);

  void update() override;

 private:
  void enableShield(component::Shield &shield, component::PhysicsBody physics);
  void disableShield(component::Shield &shield, component::PhysicsBody physics);
  b2Shape shieldShape;
};  // namespace spac::server::system
}  // namespace spac::server::system
