#include "Menu.h"
#include <string>

namespace spac::client::scene {

Menu::Menu() {
  mCamera = new Camera2D();
  mCamera->target = {0, 0};
  mCamera->offset = {0, 0};
  mCamera->zoom = 0;
  mCamera->rotation = 0;
  entt::registry mRegistry;
  mStars = new system::Starfield(mRegistry, mCamera);
}

Menu::~Menu() {
  delete mStars;
  delete mCamera;
}

void Menu::update() {}

void Menu::render() {
  auto title = "spac - " + std::to_string(GetFrameTime());
  SetWindowTitle(title.c_str());
  BeginDrawing();
  ClearBackground(BLACK);
  mCamera->target.x += 2.5;
  mCamera->target.y += 2.5;
  mStars->update();
  DrawText("Hello, world!", 190, 200, 20, LIGHTGRAY);
  EndDrawing();
}
}  // namespace spac::client::scene