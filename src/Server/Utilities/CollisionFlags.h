#pragma once

#include <cstdint>

namespace spac::server {
enum CollisionMask : uint16_t { SENSOR = 0x01, HEALTH = 0x02, DAMAGER = 0x04 };
}  // namespace spac::server