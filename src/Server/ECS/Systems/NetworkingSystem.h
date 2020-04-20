#pragma once

#include "System.h"
#include "Constants.h"
#include "CollisionFlags.h"
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <unordered_set>
#include <set>

namespace spac::server::system {
    template<bool SSL>
    class NetworkingSystem : public spac::System {
    public:
        using System::System;

        explicit NetworkingSystem(b2World *world);

        void update() override;

    private:
        b2World *mWorld;

        class NetQueryCallback : public b2QueryCallback {
        public:
            std::set<entt::entity> foundShips;
            std::set<entt::entity> foundProjectiles;

            bool ReportFixture(b2Fixture *fixture) override;
        };
    };
}


