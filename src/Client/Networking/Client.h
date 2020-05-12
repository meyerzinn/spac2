#pragma once

#include <string>
#include "Connection.h"

namespace spac::client::net {
class Client : public spac::net::Connection {
 public:
  template <typename... T>
  static std::shared_ptr<Client> create(T &&... t) {
    return std::shared_ptr<Client>(new Client(std::forward<T>(t)...));
  }
  void run(std::string host, std::string port);

 private:
  Client(asio::io_context &ioc, ssl::context &ctx);

  tcp::resolver resolver_;
  std::string host_, port_;
  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
  void on_connect(beast::error_code ec, const tcp::resolver::results_type::endpoint_type &);
  void on_handshake(beast::error_code ec);
};
// public:
//  Client(asio::io_context &ioc, ssl::context &ctx);
//  void run(std::string host, std::string port);
//  beast::error_code write(std::string_view);
//  beast::error_code read(std::string &buffer);
//
// private:
//  std::string host_;
//  std::string port_;
//  tcp::resolver resolver_;
//  websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
//  beast::flat_buffer readBuffer_;
//  std::deque<std::string> readQueue_;
//  std::deque<std::string_view> writeQueue_;
//  beast::error_code error_;
//
//  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
//  void on_connect(beast::error_code ec, const tcp::resolver::results_type::endpoint_type &);
//  void on_handshake(beast::error_code ec);
//  void on_write(beast::error_code ec, size_t bytes_transferred);
//  void on_read(beast::error_code ec, std::size_t bytes_transferred);
//
//  inline void fail(beast::error_code ec, const char *what) {
//    this->error_ = ec;
//    std::cerr << what << ":" << ec.message() << std::endl;
//  }
}  // namespace spac::client::net
