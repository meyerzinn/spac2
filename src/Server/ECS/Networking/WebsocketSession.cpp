#include "WebsocketSession.h"
#include <boost/log/trivial.hpp>

namespace spac::server::net {
WebsocketSession::WebsocketSession(tcp::socket &&socket, ssl::context &ctx)
    : Connection(websocket::stream<beast::ssl_stream<beast::tcp_stream>>(std::move(socket), ctx)) {}

void WebsocketSession::on_run() {
  BOOST_LOG_TRIVIAL(debug) << "WebsocketSession running" << std::endl;
  // Set the timeout.
  beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

  // Perform the SSL handshake
  ws_.next_layer().async_handshake(
      ssl::stream_base::server,
      beast::bind_front_handler(&WebsocketSession::on_handshake,
                                std::static_pointer_cast<WebsocketSession>(shared_from_this())));
}

void WebsocketSession::on_handshake(beast::error_code ec) {
  if (ec) return fail(*this, ec, "handshake");

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  beast::get_lowest_layer(ws_).expires_never();

  // Set suggested timeout settings for the websocket
  ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

  // Set a decorator to change the Server of the handshake
  ws_.set_option(websocket::stream_base::decorator([](websocket::response_type &res) {
    res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " server");
  }));

  // Accept the websocket handshake
  ws_.async_accept(beast::bind_front_handler(&WebsocketSession::on_accept,
                                             std::static_pointer_cast<WebsocketSession>(shared_from_this())));
}

void WebsocketSession::on_accept(beast::error_code ec) {
  if (ec) return fail(*this, ec, "accept");

  // Start the read cycle (subsequent reads are handled by the Connection abstract class)
  do_read();
}

void WebsocketSession::run() {
  asio::dispatch(ws_.get_executor(),
                 beast::bind_front_handler(&WebsocketSession::on_run,
                                           std::static_pointer_cast<WebsocketSession>(shared_from_this())));
}

}  // namespace spac::server::net