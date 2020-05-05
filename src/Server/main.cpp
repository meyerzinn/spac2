#include <box2d/box2d.h>
#include <boost/lockfree/queue.hpp>
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
#include "PerceptionsSystem.h"
#include "ShieldSystem.h"
#include <boost/filesystem.hpp>

using namespace spac::server;

int main() {
  std::cout << "Server starting..." << std::endl;

  b2World world(b2Vec2_zero);

  entt::registry registry;

  auto booster = std::make_shared<system::BoosterSystem>(registry);
  auto shield = std::make_shared<system::ShieldSystem>(registry);
  auto lifetime = std::make_shared<system::LifetimeSystem>(registry);
  auto collisions = std::make_shared<system::CollisionsSystem>(registry, world);
  auto networking = std::make_shared<system::NetworkingSystem<true>>(registry, world, uWS::Loop::get());
  auto perceptions = std::make_shared<system::PerceptionsSystem<true>>(registry, uWS::Loop::get());
  auto destruction = std::make_shared<system::DestructionSystem>(registry, world);

  std::vector<std::shared_ptr<spac::System>> systems = {booster,    shield,      lifetime,   collisions,
                                                        networking, perceptions, destruction};
  std::thread game([systems]() {
    using clock = std::chrono::high_resolution_clock;
    std::chrono::nanoseconds lag(std::chrono::nanoseconds(0));
    constexpr auto timestep = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<float, std::ratio<1>>(FIXED_SIMULATION_DURATION));
    auto time_start = clock::now();
//    unsigned int updateCounter = 0;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
      auto current_time = clock::now();
      auto delta_time = current_time - time_start;
      time_start = current_time;
      lag += delta_time;

      while (lag >= timestep) {
        lag -= timestep;
        //        auto updateStart = clock::now();
        for (const auto& system : systems) {
          system->update();
        }
        //        auto updateEnd = clock::now();
        //        updateCounter++;
        //        updateCounter %= 60;
        //        if (updateCounter == 0) {
        //          std::cout << "Update duration: " << (updateEnd - updateStart).count() << "ns" << std::endl;
        //        }
      }
    }
#pragma clang diagnostic pop
  });

  us_socket_context_options_t options{};
  options.key_file_name = "certs/key.pem";
  options.cert_file_name = "certs/cert.pem";
  std::cout << boost::filesystem::current_path() << std::endl;
  networking->listen(9002, uWS::SSLApp(options));
  game.join();
}
