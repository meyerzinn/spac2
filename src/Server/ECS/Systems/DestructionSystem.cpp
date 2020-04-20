#include "DestructionSystem.h"
#include "EntityComponents.h"

namespace spac::server {
    void system::DestructionSystem::update() {
        auto tagged = mRegistry.view<component::TaggedToDestroy>();

        mRegistry.destroy(tagged.begin(), tagged.end());
    }
}
