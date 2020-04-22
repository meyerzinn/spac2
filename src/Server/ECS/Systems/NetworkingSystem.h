#pragma once

#include <TaskQueue.h>
#include <box2d/box2d.h>
#include <uwebsockets/App.h>
#include <uwebsockets/Loop.h>
#include <uwebsockets/LoopData.h>
#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <chrono>
#include <entt/entt.hpp>
#include <set>
#include <unordered_set>
#include "CollisionFlags.h"
#include "Constants.h"
#include "System.h"
#include "packet_generated.h"

using namespace std::chrono;

namespace spac::server::system {
template <bool SSL>
class NetworkingSystem : public spac::System {
 public:
  NetworkingSystem(entt::registry &registry, b2World &world, uWS::Loop *loop);

  void update() override;

  void listen(int port, uWS::TemplatedApp<SSL> app);

 private:
  using WebSocket = uWS::WebSocket<SSL, true>;
  entt::observer mDeathObserver;
  TaskQueue mTaskQueue = TaskQueue(1024);
  b2World &mWorld;
  uWS::Loop *mLoop;

  void onMessage(WebSocket *ws, std::string_view message, uWS::OpCode opCode);
  void onOpen(WebSocket *ws, uWS::HttpRequest *req);
  void onDrain(WebSocket *ws);
  void onPing(WebSocket *ws);
  void onPong(WebSocket *ws);
  void onClose(WebSocket *ws, int code, std::string_view message);

  struct SocketData {
    entt::entity id = entt::null;
    std::shared_ptr<std::atomic<bool>> closed;
  };
};
}  // namespace spac::server::system
