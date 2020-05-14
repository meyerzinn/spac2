#include "WebsocketClient.h"
#include <boost/log/trivial.hpp>
#include <iostream>
#include <utility>

namespace spac::client::net {

WebsocketClient::WebsocketClient(asio::io_context &ioc, ssl::context &ctx)
    : resolver_(asio::make_strand(ioc)),
      Connection(websocket::stream<beast::ssl_stream<beast::tcp_stream>>(asio::make_strand(ioc), ctx)) {}

void WebsocketClient::connect(std::string host, std::string port) {
  host_ = std::move(host);
  port_ = std::move(port);
  resolver_.async_resolve(host_, port_, beast::bind_front_handler(&WebsocketClient::on_resolve, shared_from_this()));
  BOOST_LOG_TRIVIAL(debug) << "resolving host" << std::endl;
}

void WebsocketClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
  if (ec) return fail(*this, ec, "resolve");
  BOOST_LOG_TRIVIAL(debug) << "resolved host, opening connection" << std::endl;
  // set TCP timeout
  beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
  // make the connection on the IP address we get from a lookup
  beast::get_lowest_layer(ws_).async_connect(
      results, beast::bind_front_handler(&WebsocketClient::on_connect, shared_from_this()));
}

void WebsocketClient::on_connect(beast::error_code ec, const tcp::resolver::results_type::endpoint_type &ep) {
  if (ec) return fail(*this, ec, "connect");
  // TCP connection is open!
  BOOST_LOG_TRIVIAL(debug) << "starting handshake" << std::endl;

  // Update the host_ string. This will provide the value of the
  // Host HTTP header during the WebSocket handshake.
  // See https://tools.ietf.org/html/rfc7230#section-5.4
  host_ += ':' + std::to_string(ep.port());

  // Set a timeout on the operation
  beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

  // perform SSL handshake
  ws_.next_layer().async_handshake(ssl::stream_base::client,
                                   beast::bind_front_handler(&WebsocketClient::on_ssl_handshake, shared_from_this()));
}

void WebsocketClient::on_ssl_handshake(beast::error_code ec) {
  if (ec) return fail(*this, ec, "ssl_handshake");

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  beast::get_lowest_layer(ws_).expires_never();
  // Set suggested timeout settings for the websocket
  // todo adjust timeouts
  ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
  // sets user-agent
  ws_.set_option(websocket::stream_base::decorator([](websocket::request_type &req) {
    req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " spac-websocket-ssl");
  }));

  ws_.async_handshake(host_, "/", beast::bind_front_handler(&WebsocketClient::on_handshake, shared_from_this()));
}

void WebsocketClient::on_handshake(beast::error_code ec) {
  if (ec) return fail(*this, ec, "handshake");
  BOOST_LOG_TRIVIAL(debug) << "websocket handshake complete" << std::endl
                           << "websocket connection established" << std::endl;

  // Websocket connection is open! We can start sending/receiving data.

  do_read();
}

// beast::error_code WebsocketClient::write(std::string_view buffer) {
//  writeQueue_.push_back(buffer);
//  if (writeQueue_.size() <= 1)
//    ws_.async_write(asio::buffer(writeQueue_.front()),
//                    beast::bind_front_handler(&WebsocketClient::on_write, shared_from_this()));
//  return error_;
//}
//
// void WebsocketClient::on_write(beast::error_code ec, std::size_t bytes_transferred) {
//  boost::ignore_unused(bytes_transferred);
//  writeQueue_.erase(writeQueue_.begin());
//  if (ec) return fail(ec, "write");
//  if (!writeQueue_.empty()) {
//    ws_.async_write(asio::buffer(writeQueue_.front()),
//                    beast::bind_front_handler(&WebsocketClient::on_write, shared_from_this()));
//  }
//}
//
// beast::error_code WebsocketClient::read(std::string &buffer) {
//  if (!readQueue_.empty()) {
//    auto top = readQueue_.front();
//    readQueue_.pop_front();
//    buffer.assign(top);
//  }
//  return error_;
//}
//
// void WebsocketClient::on_read(beast::error_code ec, std::size_t bytes_transferred) {
//  if (ec) fail(ec, "read");
//  boost::ignore_unused(bytes_transferred);
//  readQueue_.emplace_back(asio::buffer_cast<const char *>(readBuffer_.data()));
//  ws_.async_read(readBuffer_, beast::bind_front_handler(&WebsocketClient::on_read, shared_from_this()));
//}

}  // namespace spac::client::net