#include "DestructionSystem.h"
#include "EntityComponents.h"
#include "PhysicsComponents.h"
#include "ShipComponents.h"

namespace spac::server::system {
    void DestructionSystem::update() {
        // tag all sub-entities
        auto taggedWithOwner = mRegistry.view<component::TaggedToDestroy, component::Owner>();
        for (auto entity : taggedWithOwner) {
            auto owner = mRegistry.get<component::Owner>(entity);
            mRegistry.assign<component::TaggedToDestroy>(owner.owned.begin(), owner.owned.end());
        }

        // destroy physics bodies (careful with memory management)
        auto taggedWithBody = mRegistry.view<component::PhysicsBody, component::TaggedToDestroy>();
        for (auto entity : taggedWithBody) {
            auto physics = mRegistry.get<component::PhysicsBody>(entity);
            delete static_cast<entt::entity *>(physics.body->GetUserData());
            mWorld.DestroyBody(physics.body);
        }

        // destroy physics fixtures (again, careful with memory management)
        auto taggedWithShield = mRegistry.view<component::TaggedToDestroy, component::Shield>();
        for (auto entity : taggedWithShield) {
            auto shield = mRegistry.get<component::Shield>(entity);
            if (!shield.engaged) {
                // the shield is not attached to the body, so we need to explicitly free it (maybe?)
                delete shield.fixture;
            }
        }

        auto tagged = mRegistry.view<component::TaggedToDestroy>();
        mRegistry.destroy(tagged.begin(), tagged.end());
    }

    DestructionSystem::DestructionSystem(entt::registry &registry, b2World &world) : System(registry), mWorld(world) {}
}
