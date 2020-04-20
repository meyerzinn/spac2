#include "LifetimeSystem.h"
#include <EntityComponents.h>
#include <Constants.h>

namespace spac::server::system {
    void LifetimeSystem::update() {
        auto entitiesView = mRegistry.view<component::Lifetime>();
        for (auto entity : entitiesView) {
            auto &entityLifetime = entitiesView.get<component::Lifetime>(entity);
            entityLifetime.lifetime -= FIXED_SIMULATION_DURATION;
            if (entityLifetime.lifetime < 0.f)
                mRegistry.assign<component::TaggedToDestroy>(entity);
        }
    }
}
