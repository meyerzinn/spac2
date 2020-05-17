#pragma once

#include <chrono>
#include <System.h>

namespace spac::server::system {
class PerceptionSystem : public spac::System {
 public:
  PerceptionSystem(entt::registry &registry);

  void update() override;

 private:
  std::chrono::time_point<std::chrono::system_clock> lastPerception_;
};
}  // namespace spac::server::system
