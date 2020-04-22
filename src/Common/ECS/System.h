//
// Created by meyer on 4/12/2020.
//

#pragma once

#include <entt/entt.hpp>

namespace spac {
class System {
 public:
  explicit System(entt::registry &registry);

  virtual void update() = 0;

 protected:
  entt::registry &mRegistry;
};
}  // namespace spac