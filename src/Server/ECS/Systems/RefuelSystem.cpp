#include "RefuelSystem.h"

#include <ShipComponents.h>

namespace spac::server::system {
    void RefuelSystem::update() {
        auto entities = mRegistry.view<component::Fuel>();
        for (auto entity : entities) {
            auto &fuel = mRegistry.get<component::Fuel>(entity);
            fuel.stored = std::clamp(fuel.stored + fuel.regenerate, 0.f, fuel.capacity);
            fuel.fixture->SetDensity(fuel.stored / fuel.capacity); // TODO tune this
            fuel.fixture->GetBody()->ResetMassData();
        }
    }
}
