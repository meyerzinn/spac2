#include <box2d/box2d.h>
#include <boost/certify/https_verification.hpp>
#include <boost/filesystem.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <entt/entt.hpp>
#include <iostream>
#include <thread>
#include "BoosterSystem.h"
#include "CollisionsSystem.h"
#include "Constants.h"
#include "DestructionSystem.h"
#include "LifetimeSystem.h"
#include "NetworkingSystem.h"
#include "PerceptionSystem.h"
#include "ShieldSystem.h"

using namespace spac::server;

constexpr int threads = 1;

int main() {
  BOOST_LOG_TRIVIAL(debug) << "Server starting..." << std::endl;

  asio::io_context ioc{threads};
  ssl::context ssl_ctx{ssl::context::tlsv12};
  ssl_ctx.set_verify_mode(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                          boost::asio::ssl::context::single_dh_use);
//  boost::certify::enable_native_https_server_verification(ssl_ctx);
  beast::error_code ec;
  ssl_ctx.use_certificate_chain_file("certs/cert.pem");
  ssl_ctx.use_private_key_file("certs/key.pem", ssl::context::pem, ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "could not load private key: " << ec.message() << std::endl;
    return -1;
  }
  b2World world(b2Vec2_zero);

  entt::registry registry;

  auto booster = std::make_shared<system::BoosterSystem>(registry);
  auto shield = std::make_shared<system::ShieldSystem>(registry);
  auto lifetime = std::make_shared<system::LifetimeSystem>(registry);
  auto collisions = std::make_shared<system::CollisionsSystem>(registry, world);
  auto networking = std::shared_ptr<system::NetworkingSystem>(system::NetworkingSystem::create(
      registry, world, ioc, ssl_ctx, tcp::endpoint{asio::ip::make_address("0.0.0.0"), 9004}));
  auto perceptions = std::make_shared<system::PerceptionSystem>(registry);
  auto destruction = std::make_shared<system::DestructionSystem>(registry, world);

  BOOST_LOG_TRIVIAL(debug) << "Systems initialized." << std::endl;

  networking->listen();

  std::vector<std::shared_ptr<spac::System>> systems = {booster,    shield,      lifetime,   collisions,
                                                        networking, perceptions, destruction};

  std::vector<std::thread> v;
  v.reserve(threads);
  for (auto i = 0; i < threads; i++) {
    v.emplace_back([&ioc] {
      BOOST_LOG_TRIVIAL(debug) << "Running I/O on a thread..." << std::endl;
      ioc.run();
      BOOST_LOG_TRIVIAL(debug) << "I/O thread terminated." << std::endl;
    });
  }

  BOOST_LOG_TRIVIAL(debug) << "Started networking threads." << std::endl;

  using clock = std::chrono::high_resolution_clock;
  std::chrono::nanoseconds lag(std::chrono::nanoseconds(0));
  constexpr auto timestep = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::duration<float, std::ratio<1>>(FIXED_SIMULATION_DURATION));
  auto time_start = clock::now();
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    auto current_time = clock::now();
    auto delta_time = current_time - time_start;
    time_start = current_time;
    lag += delta_time;

    while (lag >= timestep) {
      lag -= timestep;
      for (const auto& system : systems) {
        system->update();
      }
    }
  }
#pragma clang diagnostic pop
}
