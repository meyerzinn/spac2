#pragma once

#include <ECS/Systems/Starfield.h>
#include <raylib.h>
#include "Scene.h"

namespace spac::client::scene {
class Menu : public Scene {
 public:
  Menu();
  void update() override;
  void render() override;

 private:
  std::shared_ptr<Camera2D> mCamera = std::make_shared<Camera2D>();
  std::unique_ptr<system::Starfield> mStars;
  uint8_t mFrame = 0;
};
}  // namespace spac::client::scene
