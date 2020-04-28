#pragma once

#include <System.h>
#include <box2d/box2d.h>

namespace spac::server::system {

class CollisionsSystem : public spac::System, public b2ContactListener {
 public:
  CollisionsSystem(entt::registry &registry, b2World &world);

  void update() override;

  /*
   * Box2D contact handler
   */
  void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;

  void BeginContact(b2Contact *contact) override;
  void EndContact(b2Contact *contact) override;

 private:
  b2World &mWorld;

  void handleBeginSensorCollision(entt::entity entityA, entt::entity entityB);
  void handleEndSensorCollision(entt::entity entityA, entt::entity entityB);
  void handleImpulsiveCollision(entt::entity entityA, entt::entity entityB, float impulse);
};
}  // namespace spac::server::system
