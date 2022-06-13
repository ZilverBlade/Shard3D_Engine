#include "level.hpp"
#include "acting_actor.hpp"
#include "master_manager.hpp"

namespace Shard3D {
	namespace wb3d {
		Level::Level() {
			std::cout << "Creating level\n";
		}
		Level::~Level() {
			std::cout << "Destroying level\n";
			registry.clear();
		}

		Actor Level::createActor(std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { registry.create(), this };
			actor.addComponent<Components::GUIDComponent>();
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			return actor;
		}

		Actor Level::createActorWithGUID(GUID guid, std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { registry.create(), this };
			actor.addComponent<Components::GUIDComponent>(guid);
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			return actor;
		}

		// Actor Level::getActorByGUID() {}
		// Actor Level::getActorByTag() {}

		void Level::runGarbageCollector(VkDevice device) {
			if (actorKillQueue.size() != 0) {
				for (int i = 0; i < actorKillQueue.size(); i++) {
					std::cout << "Destroying actor '" << actorKillQueue.at(i).getTag() << "'\n";
					vkDeviceWaitIdle(device);
					registry.destroy(actorKillQueue.at(i));
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
					registry.remove<Components::MeshComponent>(actorKillMeshQueue.at(i));
				}
				actorKillMeshQueue.clear();
			}
		}

		void Level::captureLevel() {
		}

		void Level::reloadLevel() {
			MasterManager::loadLevel("assets/scenedata/LVLNOEDIT3D.wbu");
			killEverything();
			std::cout << "reloading level\n";
			loadRegistryCapture = true;
		}

		Actor Level::getEditorCameraActor() {
			Actor actor;
			//registry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, this.get() };
			//	if (actor.getGUID() == 0) return actor;	// editor camera actor is a special actor that always has a GUID of 0
			//});
			return actor;
		}

		void Level::begin() {
			captureLevel();
			simulationState = PlayState::Simulating;
			registry.view<Components::CppScriptComponent>().each([=](auto actor, auto& csc) {
				if (!csc.Inst) {
					csc.Inst = csc.InstScript();
					csc.Inst->aActor = Actor{ actor, this };
					csc.Inst->beginEvent();
					csc.Inst->spawnEvent();
				}
			});
		}

		void Level::tick(float dt) {
			// update scripts	
			if (simulationState == PlayState::Simulating)
				registry.view<Components::CppScriptComponent>().each([=](auto actor, auto& csc) {
				csc.Inst->tickEvent(dt);
			});
			//else std::cout << "CANNOT EXECUTE LEVEL TICK EVENT WHEN PLAYSTATE NOT PLAYING!!!\n";
			//update actor positions
		}

		void Level::end() {
			simulationState = PlayState::Stopped;
			registry.view<Components::CppScriptComponent>().each([=](auto actor, auto& csc) {
				csc.Inst->endEvent();
			});
			reloadLevel();
		}

		void Level::killActor(Actor actor) {
			actorKillQueue.emplace_back(actor);
			if (actor.hasComponent<Components::CppScriptComponent>()) {
				if (actor.getComponent<Components::CppScriptComponent>().Inst) {
					actor.getComponent<Components::CppScriptComponent>().Inst->killEvent();
					actor.getComponent<Components::CppScriptComponent>().killScript(&actor.getComponent<Components::CppScriptComponent>());
				}
			}
		}

		void Level::killMesh(Actor actor) {
			actorKillMeshQueue.emplace_back(actor);
		}

		void Level::reloadMesh(Actor actor) {
			actorReloadMeshQueue.emplace_back(actor);
		}

		void Level::killEverything() {
			registry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, this };
				if (!actor) return;
				if (!actor.hasComponent<Components::TagComponent>()) return;
				if (actor.getGUID() == 0 || actor.getGUID() == std::numeric_limits<uint64_t>::max()) return;
				killActor(actor);
			});
		}
	}
}