#pragma once
#include <Networking/WebsocketClient.h>
#include <memory>
namespace spac::client::net {
class ConnectionManager {
 public:
  static ConnectionManager* getInstance() {
    if (!instance) {
      instance = new ConnectionManager();
    }
    return instance;
  }

  void connect(std::string host, std::string port);

 private:
  inline static ConnectionManager* instance;
  std::shared_ptr<WebsocketClient> client_;

  ConnectionManager() {}
  // public:
  //  void ChangeScene(std::unique_ptr<Scene> next) { mCurrentScene = std::move(next); };
  //
  //  static SceneManager* getInstance() {
  //    if (!instance) {
  //      instance = new SceneManager();
  //    }
  //    return instance;
  //  }
  //  SceneManager(const SceneManager&) = delete;
  //  SceneManager& operator=(const SceneManager&) = delete;
  //
  //  inline void update() {
  //    if (mCurrentScene) mCurrentScene->update();
  //  }
  //
  //  inline void render() {
  //    if (mCurrentScene) mCurrentScene->render();
  //  }
  //
  // private:
  //  inline static SceneManager* instance;
};
}  // namespace spac::client::net
