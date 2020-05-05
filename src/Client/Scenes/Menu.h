#pragma once

#include <raylib.h>
#include "Scene.h"
#include "Starfield.h"

namespace spac::client::scene {
class Menu : public Scene {
 public:
  Menu();
  ~Menu() override;
  void update() override;
  void render() override;

 private:
  Camera2D* mCamera = new Camera2D();
  system::Starfield* mStars;
  uint8_t mFrame = 0;
};
}  // namespace spac::client::scene
