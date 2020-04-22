#pragma once

#include <box2d/b2_fixture.h>
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
  // accumulated fuel (kg)
  float stored = 2400;
  // maximum fuel (kg)
  float capacity = 2400;
  // fuel to regenerate per tick (kg)
  float regenerate = 100;

  // fixture stores a reference to the fuel storage container so we
  // can physically simulate the effects of ejecting fuel
  b2Fixture *fixture;
};

// Rocket booster
struct Booster {
  // stores the actual mass ejected in the last update
  float lastBurnedMass = 0;
  // the ejection speed of the fuel (m/s)
  float exhaustSpeed = 1;
  // how much mass is ejected per second (kg/s)
  float burnRate = 1.;
  // the fastest a ship can be travelling before the craft controller will not
  // allow any further ejection if the dot product of the current velocity and
  // the proposed ejection is non-negative (m/s)
  float maxLinearVelocity = 2;
};

struct Shield {
  b2Fixture *fixture;
  // how much fuel is required per second (kg/s)
  float burnRate = 1.;
};

// Rocket side thruster
struct SideBooster {
  float lastBurnedMass = 0;  // [-mass, +mass] indicates negative or positive radial direction (negative=clockwise)
  float exhaustSpeed = 0.1;
  float burnRate = 1.;
  float maxAngularVelocity = 0.1;
};
//
//    struct Shield {
//        bool active = false;
//        float damage = 100;
//        float maxDamage = 100;
//        float regenerate = 1;
//    };
}  // namespace spac::server::component