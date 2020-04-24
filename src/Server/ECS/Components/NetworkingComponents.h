#pragma once

#include <uwebsockets/App.h>
#include <atomic>
#include <unordered_set>
namespace spac::server::component {

template <bool SSL>
struct NetClient {
  explicit NetClient(uWS::WebSocket<SSL, true> *ws);

  void send(uWS::Loop *loop, std::string_view buffer);

  uWS::WebSocket<SSL, true> *conn;
  std::shared_ptr<std::atomic<bool>> closed = std::make_shared<std::atomic<bool>>(false);

  std::unordered_set<entt::entity> knownEntities;
  b2Vec2 lastPosition;
  std::chrono::time_point<std::chrono::high_resolution_clock> spawned;
};

template <bool SSL>
NetClient<SSL>::NetClient(uWS::WebSocket<SSL, true> *ws) : conn(ws) {}

template <bool SSL>
void NetClient<SSL>::send(uWS::Loop *loop, std::string_view buffer) {
  loop->defer([this, buffer]() {
    if (!closed.get()) this->conn->send(buffer);
  });
}

template class NetClient<true>;

template class NetClient<false>;

}  // namespace spac::server::component
