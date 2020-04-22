#pragma once

#include "System.h"

namespace spac::server::system {
class BoosterSystem : public spac::System {
 public:
  using spac::System::System;

  void update() override;
};
}  // namespace spac::server::system
