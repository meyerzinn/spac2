#include "LifetimeSystem.h"

#include <ECS/Components/EntityComponents.h>
#include <Utilities/Constants.h>

namespace spac::server::system {
void LifetimeSystem::update() {
  auto entities = registry_.view<component::Lifetime>();
  for (auto entity : entities) {
    auto &entityLifetime = entities.get<component::Lifetime>(entity);
    entityLifetime.lifetime -= FIXED_SIMULATION_DURATION;
    if (entityLifetime.lifetime < 0.f) registry_.assign<component::TaggedToDestroy>(entity);
  }
}
}  // namespace spac::server::system
