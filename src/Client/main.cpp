#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/certify/https_verification.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include "GameLoop.h"
#include "WebsocketClient.h"

using namespace spac::client;

int main() {
  std::cout << "Starting network connection..." << std::endl;
  boost::asio::io_context ioc;
  namespace ssl = boost::asio::ssl;
  ssl::context ssl_ctx{ssl::context::tls_client};
  ssl_ctx.set_verify_mode(ssl::context::no_sslv2 | ssl::context::verify_peer |
                          ssl::context::verify_fail_if_no_peer_cert);
  ssl_ctx.set_default_verify_paths();

  boost::certify::enable_native_https_server_verification(ssl_ctx);

  auto client = net::WebsocketClient::create(ioc, ssl_ctx);
  client->connect("localhost", "9004");
  std::thread net_thread([&ioc]() { ioc.run(); });
  std::cout << "Network connection initiated." << std::endl;
  std::cout << "Starting game loop..." << std::endl;
  Start();
  net_thread.join();
  return 0;
}