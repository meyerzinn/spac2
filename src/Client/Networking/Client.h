#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <deque>
#include <iostream>
#include <string_view>
#include <string>

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace http = beast::http;            // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace asio = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;  // from <boost/asio/ip/tcp.hpp>
namespace spac::client::net {
class Client : public std::enable_shared_from_this<Client> {
 public:
  Client(asio::io_context &ioc, ssl::context &ctx);
  void run(std::string host, std::string port);
  beast::error_code write(std::string_view);
  beast::error_code read(std::string &buffer);

 private:
  std::string host_;
  std::string port_;
  tcp::resolver resolver_;
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
  beast::flat_buffer readBuffer_;
  std::deque<std::string> readQueue_;
  std::deque<std::string_view> writeQueue_;
  beast::error_code error_;

  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
  void on_connect(beast::error_code ec, const tcp::resolver::results_type::endpoint_type &);
  void on_handshake(beast::error_code ec);
  void on_write(beast::error_code ec, size_t bytes_transferred);
  void on_read(beast::error_code ec, std::size_t bytes_transferred);

  inline void fail(beast::error_code ec, const char *what) {
    this->error_ = ec;
    std::cerr << what << ":" << ec.message() << std::endl;
  }
};
}  // namespace spac::client::net
