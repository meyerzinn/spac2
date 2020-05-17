#include <Connection.h>
#include <ECS/Systems/CollisionsSystem.h>
#include <ECS/Systems/DestructionSystem.h>
#include <ECS/Systems/NetworkingSystem.h>
#include <ECS/Systems/PerceptionSystem.h>
#include <Utilities/CollisionFlags.h>
#include <Utilities/Constants.h>
#include <box2d/box2d.h>
#include <boost/certify/https_verification.hpp>
#include <boost/filesystem.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <entt/entt.hpp>
#include <iostream>
#include <thread>

using namespace spac::server;

constexpr int threads = 2;

int main() {
  BOOST_LOG_TRIVIAL(debug) << "Server starting..." << std::endl;

  /*
  Initialize SSL for Websocket connections.
  */

  asio::io_context ioc{threads};
  ssl::context ssl_ctx{ssl::context::tlsv12};
  ssl_ctx.set_verify_mode(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                          boost::asio::ssl::context::single_dh_use);
  beast::error_code ec;
  ssl_ctx.use_certificate_chain_file("certs/cert.pem");
  ssl_ctx.use_private_key_file("certs/key.pem", ssl::context::pem, ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "could not load private key: " << ec.message() << std::endl;
    return -1;
  }

  /*
  Initialize game systems.
  */

  // construct physics world
  b2World world(b2Vec2_zero);
  // construct registry
  entt::registry registry;

  auto collisions = std::make_shared<system::CollisionsSystem>(registry, world);
  auto networking = std::shared_ptr<system::NetworkingSystem>(system::NetworkingSystem::create(
      registry, world, ioc, ssl_ctx, tcp::endpoint{asio::ip::make_address("0.0.0.0"), 9004}));
  auto perceptions = std::make_shared<system::PerceptionSystem>(registry);
  auto destruction = std::make_shared<system::DestructionSystem>(registry, world);

  // listen binds the asio async operations, so it must be run before any I/O threads are spawned. Otherwise,
  // iocontext.run() exits immediately since no work is scheduled.
  networking->listen();

  std::vector<std::shared_ptr<spac::System>> systems = {collisions, networking, perceptions, destruction};

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

  /*
  Game loop explanation:

  The game loop for the server is based on an article from Gaffer on Games called "Fix Your Timestep."
  (https://gafferongames.com/post/fix_your_timestep/)

  The basic premise is that physics does better with a fixed delta time for integration, but other systems might need to
  operate on different frequencies (i.e. rendering). Since this is a server, our "weird" system is networking; we do not
  want to send out messages for every physics update because the flood of packets will overwhelm the connections.
  Instead, when we call PerceptionSystem::update(), the system decides whether an appropriate amount of time has elapsed
  since the last update. If so, it send out a network update.

  We call each system's "update" function every time an acculumated "lag" exceeds the fixed timestep of 1/60 seconds.
  That gives the physics systems the ability to work with whole increments of the timestep while allowing other systems
  more flexibility.

  This may seem like overkill but it's meant to model good behavior for a game loop function. However, I should note
  that the lack of any "sleep" means that the game is going to consistently burn CPU cycles for at least one core, even
  with no clients connected. This is a bigger concern for running on local development machines, since on the server it
  is probably the more important process anyways.
  */

  using clock = std::chrono::high_resolution_clock;
  std::chrono::nanoseconds lag(std::chrono::nanoseconds(0));
  constexpr auto timestep = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::duration<float, std::ratio<1>>(FIXED_SIMULATION_DURATION));
  auto time_start = clock::now();
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
}
