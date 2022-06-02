#include "scene.hpp"
#include "actor.hpp"
#include "../components.hpp"


namespace Shard3D {
	namespace wb3d {
		Scene::Scene() {
			std::cout << "creating scene\n";
			entt::entity actor = eRegistry.create();
			//eRegistry.emplace<Components::TransformComponent>(actor, glm::vec3(1.f, -1.f, 1.f));
		}
		Scene::~Scene() {
			std::cout << "destroying scene\n";
			eRegistry.clear();
		}
		
		Actor Scene::createActor(const char* name) {
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>();
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			auto& tag = actor.addComponent<Components::TagComponent>();
			tag.tag = name;

			return actor;
		}

		Actor Scene::createActorWithGUID(GUID guid, const char* name) {
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>(guid);
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			auto& tag = actor.addComponent<Components::TagComponent>();
			tag.tag = name;

			return actor;
		}

		void Scene::killActor(Actor actor) {			
			eRegistry.destroy(actor);
		}

		void Scene::killEverything() {
			eRegistry.clear();
		}

		/*

		void Scene::componentAdded() { }
		*/
	}
}