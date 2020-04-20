#pragma once

namespace spac::server {
    constexpr float FIXED_SIMULATION_DURATION = 1.0 / 60.0;

    constexpr float PERCEPTION_RADIUS = 1000; // in meters

    constexpr float FUEL_DENSITY = 1141.0; // tune the density of fuel -- this is about the real-world density of liquid O2
}