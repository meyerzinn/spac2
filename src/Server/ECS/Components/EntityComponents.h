#pragma once

#include <chrono>
#include <entt/entity/fwd.hpp>
#include <vector>

namespace spac::server::component {
struct Health {
  float current;
  float max;
};

struct DealsDamage {
  float scalar = 0.5;
};

struct TaggedToDestroy {};

struct Lifetime {
  float lifetime;
};

struct Owner {
  std::vector<entt::entity> owned;
};

struct Owned {
  entt::entity owner;
};

}  // namespace spac::server::component