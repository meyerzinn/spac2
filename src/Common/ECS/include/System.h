#pragma once

#include <entt/entt.hpp>

namespace spac {
class System {
 public:
  explicit System(entt::registry &registry) : registry_(registry) {}

  virtual void update() = 0;

 protected:
  entt::registry &registry_;
};
}  // namespace spac