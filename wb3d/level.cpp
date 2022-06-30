#include "level.hpp"
#include "acting_actor.hpp" // also includes "actor.hpp"
#include "blueprint.hpp"

#include "master_manager.hpp"
#include "bpmgr.hpp"

namespace Shard3D {
	namespace wb3d {
		Level::Level(std::string lvlName) {}
		Level::~Level() {
			SHARD3D_INFO("Destroying level");
			registry.clear();
		}

		template<typename Component>
		static void copyComponent(entt::registry& dst, entt::registry& src, std::unordered_map<GUID, entt::entity>& map) {
			auto view = src.view<Component>();
			for (auto e : view) {
				GUID guid = src.get<Components::GUIDComponent>(e).id;
				entt::entity dstEnttID = map.at(guid);
				
				auto& component = src.get<Component>(e);

				dst.emplace_or_replace<Component>(dstEnttID, component);
			}
		}

		std::shared_ptr<Level> Level::copy(std::shared_ptr<Level> other) {
			std::shared_ptr<Level> newLvl = std::make_shared<Level>(other->name);
			std::unordered_map<GUID, entt::entity> enttMap;

			auto& srcLvlRegistry = other->registry;
			auto& dstLvlRegistry = newLvl->registry;

			auto idView = srcLvlRegistry.view<Components::GUIDComponent>();
			for (auto e : idView) {
				GUID guid = srcLvlRegistry.get<Components::GUIDComponent>(e).id;
				Actor newActor = newLvl->createActorWithGUID(guid, srcLvlRegistry.get<Components::TagComponent>(e).tag);
				enttMap[guid] = (entt::entity)newActor;
			}

			copyComponent<Components::BlueprintComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::TransformComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::MeshComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::CameraComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::DirectionalLightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::PointlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::SpotlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::CppScriptComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);

			newLvl->setPossessedCameraActor(other->getPossessedCameraActor());
			
			return newLvl;
		}

		Blueprint Level::createBlueprint(Actor actor, std::string path, std::string name) {
			
			Blueprint blueprint = { actor, path, name };
			BlueprintManager bpMan = actor;
			blueprint.attach(actor);
			bpMan.convert(path, blueprint);

			return blueprint;
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
					SHARD3D_LOG("Destroying actor '{0}'", actorKillQueue.at(i).getTag());
					if (actorKillQueue.at(i).hasComponent<Components::MeshComponent>()) vkDeviceWaitIdle(device);
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

		void Level::setPossessedCameraActor(Actor actor) {
			if (!actor.hasComponent<Components::CameraComponent>()) {
				SHARD3D_FATAL("Can't possess a non camera actor!!");
			}
			possessedCameraActorGUID = actor.getGUID();
		}

		void Level::setPossessedCameraActor(GUID guid) {
			possessedCameraActorGUID = guid;
		}

		Actor Level::getPossessedCameraActor() {
			auto guidView = registry.view<Components::GUIDComponent>();
			for (auto actor : guidView) {
				if (guidView.get<Components::GUIDComponent>(actor).id == possessedCameraActorGUID) {
					//std::cout << "Using possessed: 0x" << &camView.get<Components::CameraComponent>(actor).camera << "\n";
					return Actor{actor, this}; 
				}
			}
			SHARD3D_FATAL("No possessed camera found!!!!");
			SHARD3D_ERROR("Attempted to find GUID: {0}", possessedCameraActorGUID);
		}

		 EngineCamera& Level::getPossessedCamera() {
			return getPossessedCameraActor().getComponent<Components::CameraComponent>().camera;
		 }

		void Level::begin() {
			SHARD3D_LOG("Beginning simulation");
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
			SHARD3D_LOG("Ending simulation");
			simulationState = PlayState::Stopped;
			registry.view<Components::CppScriptComponent>().each([=](auto actor, auto& csc) {
				csc.Inst->endEvent();
			});
			SHARD3D_INFO("Reloading level");
			loadRegistryCapture = true;
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
				if (actor.getGUID() == 0 || actor.getGUID() == UINT64_MAX) return;
				killActor(actor);
			});
		}
	}
}