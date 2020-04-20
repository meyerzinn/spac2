#pragma once

#include <cstdint>

namespace spac::server {
    enum EntityCategory : uint16_t {
        SHIP = 0x0001,
        PROJECTILE = 0x0002,
        SHIELD = 0x0003
    };


    enum EntityMask : uint16_t {
        SHIP_MASK = EntityCategory::SHIP | EntityCategory::PROJECTILE,
        PROJECTILE_MASK = EntityCategory::SHIP | EntityCategory::PROJECTILE | EntityCategory::SHIELD,
        SHIELD_MASK = EntityCategory::PROJECTILE
    };
}