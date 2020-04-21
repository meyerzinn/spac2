#pragma once

#include <vector>
#include <entt/entity/fwd.hpp>

namespace spac::server::component {
    struct Health {
        float current;
        float max;
    };

    struct DealsDamage {
        float scalar = 0.5;
    };

    struct TaggedToDestroy {
    };

    struct Lifetime {
        float lifetime;
    };

    struct Owner {
        std::vector<entt::entity> owned;
    };

    struct Owned {
        entt::entity owner;
    };
}