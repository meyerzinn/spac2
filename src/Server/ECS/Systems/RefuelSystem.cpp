#include "RefuelSystem.h"

#include <Constants.h>
#include <ShipComponents.h>

namespace spac::server::system {

// todo tune the density of fuel -- this is about the real-world density of liquid O2
constexpr float FUEL_DENSITY = 1141.0;

void RefuelSystem::update() {
  auto entities = mRegistry.view<component::Fuel>();
  for (auto entity : entities) {
    auto &fuel = entities.get<component::Fuel>(entity);
    fuel.stored = std::clamp(fuel.stored + fuel.regenerate, 0.f, fuel.capacity);
    fuel.fixture->SetDensity(fuel.stored / fuel.capacity * FUEL_DENSITY);
    fuel.fixture->GetBody()->ResetMassData();
  }
}
}  // namespace spac::server::system
