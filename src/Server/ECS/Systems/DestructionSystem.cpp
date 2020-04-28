#include "DestructionSystem.h"

namespace spac::server::system {
void DestructionSystem::update() {
  // tag all sub-entities
  auto taggedWithOwner = mRegistry.view<component::TaggedToDestroy, component::Owner>();
  for (auto entity : taggedWithOwner) {
    auto owner = mRegistry.get<component::Owner>(entity);
    mRegistry.assign<component::TaggedToDestroy>(owner.owned.begin(), owner.owned.end());
  }

  auto taggedWithOwned = mRegistry.view<component::TaggedToDestroy, component::Owned>();
  for (auto entity : taggedWithOwned) {
    auto ownerEntity = taggedWithOwned.get<component::Owned>(entity).owner;
    auto owner = mRegistry.get<component::Owner>(entity);
    owner.owned.erase(std::remove(owner.owned.begin(), owner.owned.end(), entity), owner.owned.end());
  }

  // destroy physics bodies (careful with memory management)
  auto taggedWithBody = mRegistry.view<component::PhysicsBody, component::TaggedToDestroy>();
  for (auto entity : taggedWithBody) {
    auto physics = mRegistry.get<component::PhysicsBody>(entity);
    delete static_cast<entt::entity *>(physics.body->GetUserData());
    mWorld.DestroyBody(physics.body);
  }

  auto tagged = mRegistry.view<component::TaggedToDestroy>();
  mRegistry.destroy(tagged.begin(), tagged.end());
}

DestructionSystem::DestructionSystem(entt::registry &registry, b2World &world) : System(registry), mWorld(world) {}
}  // namespace spac::server::system
