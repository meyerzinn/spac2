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
  mFrame++;

  BeginDrawing();
  ClearBackground(BLACK);
  mCamera->target.x += 2.5;
  mCamera->target.y += 2.5;
  mStars->update();
  const char title[5] = "spac";
  const int titleFontSize = 120;

  const char prompt[27] = "Press [ENTER] to continue.";
  const int promptFontSize = 36;

  const int titlePosX = GetScreenWidth() / 2 - MeasureText(title, titleFontSize) / 2;
  const int titlePosY = GetScreenHeight() / 2 - titleFontSize;
  const int promptPosX = GetScreenWidth() / 2 - MeasureText(prompt, promptFontSize) / 2;
  const int promptPosY = GetScreenHeight() / 2 + 10;
  DrawText(title, titlePosX, titlePosY, titleFontSize, WHITE);
  DrawText(prompt, promptPosX, promptPosY, promptFontSize, WHITE);
  //  DrawTextEx()
  //  DrawText(title, GetScreenWidth())
  //  DrawText(text, GetScreenWidth() / 2 - MeasureText(text, fontSize)/2, GetScreenHeight()/2 - fontSize / 2,
  //  fontSize, LIGHTGRAY);
  EndDrawing();
}
}  // namespace spac::client::scene