#include <iostream>
#include <thread>
#include <boost/lockfree/queue.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

#include "LifetimeSystem.h"
#include "NetworkingSystem.h"
#include "CollisionsSystem.h"
#include "DestructionSystem.h"

using namespace spac::server;

int main() {
    std::cout << "Server starting..." << std::endl;

    b2World world(b2Vec2_zero);

    entt::registry registry;

    auto lifetime = system::LifetimeSystem(registry);
    auto collisions = system::CollisionsSystem(registry, world);
    auto networking = system::NetworkingSystem<true>(registry, world);
    auto destruction = system::DestructionSystem(registry, world);

    networking.listen(9001, uWS::SSLApp(
            {
                    .key_file_name="certs/key.pem",
                    .cert_file_name="carts/cert.pem"
            }
    ));
    return 0;
}
