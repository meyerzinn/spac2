#pragma once

#include <System.h>

namespace spac::server::system {

class RefuelSystem : public spac::System {
  using System::System;

  void update() override;
};
}  // namespace spac::server::system
