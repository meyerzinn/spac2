#pragma once

#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <boost/container/flat_set.hpp>

namespace spac::server::component {

struct PhysicsBody {
  b2Body *body;
};

struct Sensing {
  boost::container::flat_set<entt::entity> entities;
};

}  // namespace spac::server::component