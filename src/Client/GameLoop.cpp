#include <Menu.h>
#include <raylib.h>
#include <chrono>
#include <iostream>
#include <string>
#include "SceneManager.h"

namespace spac::client {

void Start() {
  using clock = std::chrono::high_resolution_clock;

  std::cout << "Hello, world!" << std::endl;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
  InitWindow(1024, 768, "spac");
  SetTargetFPS(60);
  SetWindowMinSize(640, 480);
  SceneManager::getInstance()->ChangeScene(new scene::Menu());

  using clock = std::chrono::high_resolution_clock;
  std::chrono::nanoseconds lag(std::chrono::nanoseconds(0));
  constexpr auto timestep = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(16));
  auto time_start = clock::now();

  std::string title;
  while (!WindowShouldClose()) {
    auto current_time = clock::now();
    auto delta_time = current_time - time_start;
    time_start = current_time;
    lag += delta_time;

    while (lag >= timestep) {
      lag -= timestep;
      SceneManager::getInstance()->update();  // execute fixed timestep updates (i.e. interpolation, etc.)
    }
    // render
    title = "spac - " + std::to_string(GetFrameTime());
    SetWindowTitle(title.c_str());
    SceneManager::getInstance()->render();
  }

  CloseWindow();
}
}  // namespace spac::client