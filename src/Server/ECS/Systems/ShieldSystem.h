#pragma once

#include "PhysicsComponents.h"
#include "ShipComponents.h"
#include "System.h"

namespace spac::server::system {
class ShieldSystem : public spac::System {
 public:
  using System::System;

  void update() override;

 private:
  void enableShield(entt::entity entity, component::Shielded &shielded);
  void disableShield(component::Shielded &shielded);
};  // namespace spac::server::system
}  // namespace spac::server::system
