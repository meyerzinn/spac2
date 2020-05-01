#include <raylib.h>
#include <chrono>
#include <cmath>
#include <entt/entt.hpp>
#include <iostream>
#include "Menu.h"
#include "SceneManager.h"

using namespace spac::client;
int main() {
  using clock = std::chrono::high_resolution_clock;

  std::cout << "Hello, world!" << std::endl;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(GetScreenWidth(), GetScreenHeight(), "spac");
  SetTargetFPS(60);
  SceneManager::getInstance()->ChangeScene(new scene::Menu());

  while (!WindowShouldClose()) {
    SceneManager::getInstance()->update();
    SceneManager::getInstance()->render();
  }

  CloseWindow();
  return 0;
}