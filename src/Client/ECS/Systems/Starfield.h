#pragma once

#include <System.h>
#include <raylib.h>

namespace spac::client::system {
class Starfield : public System {
 public:
  Starfield(entt::registry& registry, std::shared_ptr<Camera2D> camera);

  void update() override;

 private:
  std::shared_ptr<Camera2D> mCamera;
  static int32_t mix(int32_t a, int32_t b, int32_t c);
  static void draw(Rectangle bounds, int32_t scale, Color color);
};
}  // namespace spac::client::system
