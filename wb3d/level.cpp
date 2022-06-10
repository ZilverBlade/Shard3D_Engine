#include "level.hpp"
#include "acting_actor.hpp"

namespace Shard3D {
	namespace wb3d {
		Level::Level() {
			std::cout << "Creating level\n";
		}
		Level::~Level() {
			std::cout << "Destroying level\n";
			eRegistry.clear();
		}
		
		Actor Level::createActor(std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>();
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			return actor;
		}

		Actor Level::createActorWithGUID(GUID guid, std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>(guid);
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			return actor;
		}

		Actor Level::createChild(Actor actor, std::string name) {
			Actor newActor = createActor(name);
			actor.relationship.children.push_back(newActor);
			return newActor;
		}

		Actor Level::createChildWithGUID(GUID guid, Actor actor, std::string name) {
			Actor newActor = createActorWithGUID(guid, name);
			actor.relationship.children.push_back(newActor);
			return newActor;
		}
		/*template<typename T>
		Actor Level::createActorWithComponent(std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			actor.addComponent<T>();
			return actor;
		}
		*/
		void Level::runGarbageCollector(VkDevice device) {
			if (actorKillQueue.size() != 0) {
				for (int i = 0; i < actorKillQueue.size(); i++) {
					std::cout << "Destroying actor '" << actorKillQueue.at(i).getTag() << "'\n";
					vkDeviceWaitIdle(device);
					eRegistry.destroy(actorKillQueue.at(i));
				}
				actorKillQueue.clear();
				return;
			}
			if (actorReloadMeshQueue.size() != 0) {
				for (int i = 0; i < actorReloadMeshQueue.size(); i++) {
					vkDeviceWaitIdle(device);
					actorReloadMeshQueue.at(i).getComponent<Components::MeshComponent>().reapplyModel(actorReloadMeshQueue.at(i).getComponent<Components::MeshComponent>().newModel);
				}
				actorReloadMeshQueue.clear();
				return;
			}
			if (actorKillMeshQueue.size() != 0) {
				for (int i = 0; i < actorKillMeshQueue.size(); i++) {
					vkDeviceWaitIdle(device);
					eRegistry.remove<Components::MeshComponent>(actorKillMeshQueue.at(i));
				}
				actorKillMeshQueue.clear();
			}
		}

		Actor Level::getEditorCameraActor() {
			Actor actor;
			//eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, this.get() };
			//	if (actor.getGUID() == 0) return actor;	// editor camera actor is a special actor that always has a GUID of 0
			//});
			return actor;
		}

		void Level::begin() {
			eRegistry.view<Components::CppScriptComponent>().each([=](auto actor, auto& csc) {
				if (!csc.Inst) {
					csc.Inst = csc.InstScript();
					csc.Inst->aActor = Actor{ actor, this };
					csc.Inst->beginEvent();
				}
			});
		}

		void Level::update(float dt) {
			
			{	// update scripts	
				eRegistry.view<Components::CppScriptComponent>().each([=](auto actor, auto& csc) {
					if (!csc.Inst) {
						csc.Inst = csc.InstScript();
						csc.Inst->aActor = Actor{ actor, this };
						csc.Inst->spawnEvent();
					}
					csc.Inst->tickEvent(dt);
				});
			}
		}

		void Level::end() {

		}

		void Level::killActor(Actor actor) {
			actorKillQueue.emplace_back(actor);
			if (actor.hasComponent<Components::CppScriptComponent>()) {
				actor.getComponent<Components::CppScriptComponent>().Inst->killEvent();
				actor.getComponent<Components::CppScriptComponent>().killScript(&actor.getComponent<Components::CppScriptComponent>());
			}
		}

		void Level::killChild(Actor parentActor, Actor childActor) {
			if (std::find(parentActor.relationship.children.begin(), parentActor.relationship.children.end(), childActor) != parentActor.relationship.children.end()) {
				actorKillQueue.emplace_back(childActor);
				//parentActor.relationship.children.erase(childActor);
			}
		}

		void Level::killMesh(Actor actor) {
			actorKillMeshQueue.emplace_back(actor);
		}

		void Level::reloadMesh(Actor actor) {
			actorReloadMeshQueue.emplace_back(actor);
		}

		void Level::killEverything() {
			eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, this };
				if (!actor) return;
				if (!actor.hasComponent<Components::TagComponent>()) return;
				if (actor.getGUID() == 0 || actor.getGUID() == std::numeric_limits<uint64_t>::max()) return;
				killActor(actor);
			});
		}


		/*

		void Level::componentAdded() { }
		*/
	}
}