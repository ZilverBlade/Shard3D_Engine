#include "scene.hpp"
#include "actor.hpp"


namespace Shard3D {
	namespace wb3d {
		Scene::Scene() {
			entt::entity actor = eRegistry.create();
			eRegistry.emplace<TransformComponent>(actor, glm::vec3(1.f, -1.f, 1.f));
		}
		Scene::~Scene() {
			eRegistry.clear();
		}
		
		Actor Scene::createActor(std::string name) {
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<TransformComponent>();
			auto& tag = actor.addComponent<TagComponent>();
			tag.tag = name;

			return actor;
		}
		void Scene::killActor(Actor actor) {
			
			// eRegistry.destroy(actor)
		}
		/*
		void Scene::killEntity(Entity entity) {
		}
		

		void Scene::componentAdded() { }
		*/
	}
}