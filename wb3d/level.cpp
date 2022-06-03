#include "level.hpp"
#include "actor.hpp"
#include "../components.hpp"


namespace Shard3D {
	namespace wb3d {
		Level::Level() {
			std::cout << "Creating level\n";
			entt::entity actor = eRegistry.create();
			//eRegistry.emplace<Components::TransformComponent>(actor, glm::vec3(1.f, -1.f, 1.f));
		}
		Level::~Level() {
			std::cout << "Destroying level\n";
			eRegistry.clear();
		}
		
		Actor Level::createActor(std::string  name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>();
			auto& tag = actor.addComponent<Components::TagComponent>();
			tag.tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			return actor;
		}

		Actor Level::createActorWithGUID(GUID guid, std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>(guid);
			auto& tag = actor.addComponent<Components::TagComponent>();
			tag.tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif

			return actor;
		}

		void Level::killActor(Actor actor) {			
			eRegistry.destroy(actor);
		}

		void Level::killEverything() {
			eRegistry.clear();
		}

		/*

		void Level::componentAdded() { }
		*/
	}
}