#pragma once

#include <boost/container/flat_set.hpp>

namespace spac::server::component {

struct Perceivable {
  enum Kind { SHIP, PROJECTILE } kind;
};

struct Sensing {
  boost::container::flat_set<entt::entity> entities;
};

}  // namespace spac::server::component
