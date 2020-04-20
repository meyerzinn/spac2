#pragma once

#include <box2d/box2d.h>

namespace spac::server::component {

    struct PhysicsBody {
        b2Body *body;
    };

}