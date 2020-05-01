#pragma once

#include <memory>
#include "Scene.h"
namespace spac::client {
class SceneManager {
 public:
  void ChangeScene(Scene* next) {
    delete mCurrentScene;  // free memory associated with the scene
    mCurrentScene = next;
  };

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
  SceneManager() : mCurrentScene(nullptr){};
  Scene* mCurrentScene;
};
}  // namespace spac::client
