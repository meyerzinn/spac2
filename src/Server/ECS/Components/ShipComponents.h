#pragma once

#include <box2d/b2_fixture.h>
#include <chrono>
#include <entt/entt.hpp>
#include <string>

namespace spac::server::component {
struct Named {
  std::string name;
};

struct ShipController {
  float booster = 0.;       // on [0, 1]
  float sideThruster = 0.;  // on [-1, 1]
  bool shield = false;
};

struct Fuel {
  // fixture stores a reference to the fuel storage container so we
  // can physically simulate the effects of ejecting fuel
  b2Fixture *fixture;

  // accumulated fuel (kg)
  float stored = 2400.0f;
  // maximum fuel (kg)
  float capacity = 2400.0f;
  // fuel to regenerate per tick (kg)
  float regenerate = 100.0f;
};

// Rocket booster
struct Booster {
  // stores the actual mass ejected in the last update
  float lastBurnedMass = 0.0f;
  // the ejection speed of the fuel (m/s)
  float exhaustSpeed = 1.0f;
  // how much mass is ejected per second (kg/s)
  float burnRate = 1.0f;
  // the fastest a ship can be travelling before the craft controller will not
  // allow any further ejection if the dot product of the current velocity and
  // the proposed ejection is non-negative (m/s)
  float maxLinearVelocity = 2.0f;
};

struct Shielded {
  b2Fixture *fixture;
  entt::entity shield = entt::null;  // create a new Sensing entity every time the shield is engaged
  // how much fuel is required to keep the shield engaged (kg/s)
  float burnRate = 1.0f;
};

// Rocket side thruster
struct SideBooster {
  float lastBurnedMass = 0.0f;  // [-mass, +mass] indicates negative or positive radial direction (negative=clockwise)
  float exhaustSpeed = 0.1f;
  float burnRate = 1.0f;
  float maxAngularVelocity = 0.1f;
};

}  // namespace spac::server::component