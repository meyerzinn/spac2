#pragma once

#include "System.h"
#include "Constants.h"
#include "CollisionFlags.h"
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <unordered_set>
#include <set>
#include <boost/lockfree/queue.hpp>
#include <TaskQueue.h>
#include <uwebsockets/App.h>
#include <chrono>

using namespace std::chrono;

namespace spac::server::system {
    template<bool SSL>
    class NetworkingSystem : public spac::System {
    public:
        NetworkingSystem(entt::registry &registry, b2World &world);

        void update() override;

        void listen(int port, uWS::TemplatedApp<SSL> app);

    private:
        TaskQueue mTaskQueue = TaskQueue(1024);
        b2World &mWorld;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastPerception;

        static void onNetClientComponentDestroyed(NetworkingSystem *ns, entt::registry &registry, entt::entity entity);

        class NetQueryCallback : public b2QueryCallback {
        public:
            std::set<entt::entity> foundShips;
            std::set<entt::entity> foundProjectiles;

            bool ReportFixture(b2Fixture *fixture) override;
        };
    };
}

