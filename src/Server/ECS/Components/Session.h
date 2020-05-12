#pragma once

#include <boost/container/flat_set.hpp>
#include <chrono>
#include <entt/entt.hpp>
#include "WebsocketSession.h"

namespace spac::server::component {


struct SessionComponent {
  explicit SessionComponent(std::shared_ptr<spac::server::net::WebsocketSession> session);

  std::shared_ptr<spac::server::net::WebsocketSession> conn;
  boost::container::flat_set<entt::entity> known;
  std::chrono::time_point<std::chrono::high_resolution_clock> spawned;
};
}  // namespace spac::server::component