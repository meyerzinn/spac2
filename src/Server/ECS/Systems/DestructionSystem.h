#pragma once

#include <box2d/box2d.h>
#include "System.h"
#include "EntityComponents.h"
#include "PhysicsComponents.h"

namespace spac::server::system {
class DestructionSystem : public spac::System {
 public:
  DestructionSystem(entt::registry &registry, b2World &world);

  void update() override;

 private:
  b2World &mWorld;
};
}  // namespace spac::server::system
