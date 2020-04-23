#include "LifetimeSystem.h"
#include <Constants.h>
#include <EntityComponents.h>

namespace spac::server::system {
void LifetimeSystem::update() {
  auto entities = mRegistry.view<component::Lifetime>();
  for (auto entity : entities) {
    auto &entityLifetime = entities.get<component::Lifetime>(entity);
    entityLifetime.lifetime -= FIXED_SIMULATION_DURATION;
    if (entityLifetime.lifetime < 0.f) mRegistry.assign<component::TaggedToDestroy>(entity);
  }
}
}  // namespace spac::server::system
