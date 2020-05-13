#pragma once

#include <cstdint>

namespace spac::server {
enum CollisionMask : uint16_t { SENSOR = 0x01u, HEALTH = 0x02u, DAMAGER = 0x04u };
}  // namespace spac::server