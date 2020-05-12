#pragma once

#include <box2d/b2_world.h>
#include "Session.h"
#include "System.h"
#include "packet_generated.h"

using namespace std::chrono;

namespace spac::server::system {

class NetworkingSystem : public spac::System, public std::enable_shared_from_this<NetworkingSystem> {
 public:
  template <typename... T>
  static std::shared_ptr<NetworkingSystem> create(T &&... t) {
    return std::shared_ptr<NetworkingSystem>(new NetworkingSystem(std::forward<T>(t)...));
  }

  void update() override;
  void listen();

 private:
  NetworkingSystem(entt::registry &registry, b2World &world, asio::io_context &ioc, ssl::context &ctx,
                   tcp::endpoint endpoint);

  entt::observer deathObserver_;
  b2World &world_;
  asio::io_context &ioc_;
  ssl::context &ctx_;
  tcp::acceptor acceptor_;

  void do_accept();
  void on_accept(beast::error_code ec, tcp::socket socket);

  void handleRespawn(entt::entity entity, const ::spac::net::Packet *packet);
  //  void handleInputs();
};
}  // namespace spac::server::system
