#pragma once

#include <memory>
#include "Scene.h"

namespace spac::client {
class SceneManager {
 public:
  void ChangeScene(std::unique_ptr<Scene> next) { mCurrentScene = std::move(next); };

  static SceneManager* getInstance() {
    if (!instance) {
      instance = new SceneManager();
    }
    return instance;
  }
  SceneManager(const SceneManager&) = delete;
  SceneManager& operator=(const SceneManager&) = delete;

  inline void update() {
    if (mCurrentScene) mCurrentScene->update();
  }

  inline void render() {
    if (mCurrentScene) mCurrentScene->render();
  }

 private:
  inline static SceneManager* instance;
  SceneManager() = default;
  std::unique_ptr<Scene> mCurrentScene;
};
}  // namespace spac::client
