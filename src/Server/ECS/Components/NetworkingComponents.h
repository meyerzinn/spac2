#pragma once

#include <uwebsockets/App.h>
#include <atomic>
#include <unordered_set>
#include <box2d/b2_math.h>
#include <entt/fwd.hpp>

namespace spac::server::component {

struct Perceivable {
  enum Kind { SHIP, PROJECTILE } kind;
};

template <bool SSL>
struct NetClient {
  explicit NetClient(uWS::WebSocket<SSL, true> *ws);

  void send(uWS::Loop *loop, std::string_view buffer);

  uWS::WebSocket<SSL, true> *conn;
  std::shared_ptr<std::atomic<bool>> closed = std::make_shared<std::atomic<bool>>(false);

  std::unordered_set<entt::entity> knownEntities{};
  b2Vec2 lastPosition{};
  std::chrono::time_point<std::chrono::high_resolution_clock> spawned;
};

template struct NetClient<true>;

template struct NetClient<false>;

}  // namespace spac::server::component
