#include "Client.h"
#include <boost/log/trivial.hpp>
#include <iostream>
#include <utility>

namespace spac::client::net {

Client::Client(asio::io_context &ioc, ssl::context &ctx)
    : resolver_(asio::make_strand(ioc)), ws_(asio::make_strand(ioc), ctx) {}

void Client::run(std::string host, std::string port) {
  host_ = std::move(host);
  port_ = std::move(port);
  resolver_.async_resolve(host_, port_, beast::bind_front_handler(&Client::on_resolve, shared_from_this()));
  BOOST_LOG_TRIVIAL(debug) << "async resolve..." << std::endl;
}
void Client::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
  if (ec) return fail(ec, "resolve");
  BOOST_LOG_TRIVIAL(debug) << "resolved successfully" << std::endl;
  // set TCP timeout
  beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
  // make the connection on the IP address we get from a lookup
  beast::get_lowest_layer(ws_).async_connect(results,
                                             beast::bind_front_handler(&Client::on_connect, shared_from_this()));
}
void Client::on_connect(beast::error_code ec, const tcp::resolver::results_type::endpoint_type &) {
  if (ec) return fail(ec, "connect");

  // TCP connection is open!

  // turn off TCP timeout
  beast::get_lowest_layer(ws_).expires_never();
  // pings should be a last resort anyways, since there should always be client-server traffic
  ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
  // sets user-agent
  ws_.set_option(websocket::stream_base::decorator([](websocket::request_type &req) {
    req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client");
  }));
  // perform websocket handshake
  ws_.async_handshake(host_, "/", beast::bind_front_handler(&Client::on_handshake, shared_from_this()));
}

void Client::on_handshake(beast::error_code ec) {
  if (ec) return fail(ec, "handshake");

  // Websocket connection is open! We can start sending/receiving data.
  ws_.async_read(readBuffer_, beast::bind_front_handler(&Client::on_read, shared_from_this()));
}

beast::error_code Client::write(std::string_view buffer) {
  writeQueue_.push_back(buffer);
  if (writeQueue_.size() <= 1)
    ws_.async_write(asio::buffer(writeQueue_.front()),
                    beast::bind_front_handler(&Client::on_write, shared_from_this()));
  return error_;
}

void Client::on_write(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  writeQueue_.erase(writeQueue_.begin());
  if (ec) return fail(ec, "write");
  if (!writeQueue_.empty()) {
    ws_.async_write(asio::buffer(writeQueue_.front()),
                    beast::bind_front_handler(&Client::on_write, shared_from_this()));
  }
}

beast::error_code Client::read(std::string &buffer) {
  if (!readQueue_.empty()) {
    auto top = readQueue_.front();
    readQueue_.pop_front();
    buffer.assign(top);
  }
  return error_;
}

void Client::on_read(beast::error_code ec, std::size_t bytes_transferred) {
  if (ec) fail(ec, "read");
  boost::ignore_unused(bytes_transferred);
  readQueue_.emplace_back(asio::buffer_cast<const char *>(readBuffer_.data()));
  ws_.async_read(readBuffer_, beast::bind_front_handler(&Client::on_read, shared_from_this()));
}

}  // namespace spac::client::net