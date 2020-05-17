#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <Connection.h>

namespace spac::server::net {
class WebsocketSession : public spac::net::Connection<WebsocketSession> {
 public:
  template <typename... T>
  static std::shared_ptr<WebsocketSession> create(T &&... t) {
    return std::shared_ptr<WebsocketSession>(new WebsocketSession(std::forward<T>(t)...));
  }

  void run();

 private:
  WebsocketSession(tcp::socket &&socket, ssl::context &ctx);

  void on_run();

  void on_handshake(beast::error_code ec);

  void on_accept(beast::error_code ec);
};
}  // namespace spac::server::net