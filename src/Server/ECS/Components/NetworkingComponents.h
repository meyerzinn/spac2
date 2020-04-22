#pragma once

#include <uwebsockets/App.h>
#include <atomic>
#include <unordered_set>
namespace spac::server::component {

template <bool SSL>
struct NetClient {
  NetClient(uWS::WebSocket<SSL, true> *ws);

  void send(std::string_view buffer);

  uWS::WebSocket<SSL, true> *conn;
  std::shared_ptr<std::atomic<bool>> closed = std::make_shared<std::atomic<bool>>(false);

  std::unordered_set<entt::entity> knownEntities;
  b2Vec2 lastPosition;
  std::chrono::time_point<std::chrono::high_resolution_clock> spawned;
};

template <bool SSL>
NetClient<SSL>::NetClient(uWS::WebSocket<SSL, true> *ws) : conn(ws) {}

template <bool SSL>
void NetClient<SSL>::send(std::string_view buffer) {
  if (closed->operator bool()) return;
  conn->send(buffer);
}

template class NetClient<true>;

template class NetClient<false>;

}  // namespace spac::server::component
