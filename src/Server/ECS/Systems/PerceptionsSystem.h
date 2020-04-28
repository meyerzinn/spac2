#pragma once

#include <box2d/box2d.h>
#include <uwebsockets/Loop.h>
#include <entt/entity/fwd.hpp>
#include <set>
#include "System.h"

namespace spac::server::system {
template <bool SSL>
class PerceptionsSystem : public spac::System {
 public:
  PerceptionsSystem(entt::registry &registry, /*b2World &world,*/ uWS::Loop *loop);

  void update() override;

 private:
//  b2World &mWorld;
  uWS::Loop *mLoop;
  std::chrono::time_point<std::chrono::system_clock> mLastPerception;

//  class NetQueryCallback : public b2QueryCallback {
//   public:
//    std::set<entt::entity> foundShips;
//    std::set<entt::entity> foundProjectiles;
//
//    bool ReportFixture(b2Fixture *fixture) override;
//  };
};
}  // namespace spac::server::system
