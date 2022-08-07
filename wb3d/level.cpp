#include "../s3dtpch.h" 
#include "level.hpp"
#include "acting_actor.hpp" // also includes "actor.hpp"
#include "blueprint.hpp"

#include "master_manager.hpp"
#include "bpmgr.hpp"
#include "assetmgr.hpp"
#include "../scripts/dynamic_script_engine.hpp"
#include "../singleton.hpp"
#include "../hud.hpp"
namespace Shard3D {
	namespace wb3d {
		Level::Level(const std::string &lvlName) : name(lvlName) {}
		Level::~Level() {
			SHARD3D_INFO("Destroying level");
			registry.clear();
		}

		template<typename Component>
		static void copyComponent(entt::registry& dst, entt::registry& src, std::unordered_map<GUID, entt::entity>& map) {
			auto view = src.view<Component>();
			for (auto e : view) {
				GUID guid = src.get<Components::GUIDComponent>(e).id;
				if (map.find(guid) == map.end()) SHARD3D_FATAL("enttMap.find(guid) == enttMap.end()");

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
			copyComponent<Components::BillboardComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::MeshComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::AudioComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::CameraComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::DirectionalLightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::PointlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::SpotlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::CppScriptComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::ScriptComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);

			newLvl->possessedCameraActorGUID = other->getPossessedCameraActor().getGUID();
			
			return newLvl;
		}

		Blueprint Level::createBlueprint(Actor actor, std::string path, std::string name) {
			
			Blueprint blueprint = { actor, path, name };
			BlueprintManager bpMan = actor;
			blueprint.attach(actor);
			bpMan.convert(path, blueprint);

			return blueprint;
		}

		Actor Level::createActor(const std::string& name) {
			return createActorWithGUID(GUID(), name);
		}

		Actor Level::createActorWithGUID(GUID guid, const std::string& name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { registry.create(), this };
			actor.addComponent<Components::GUIDComponent>(guid);
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ENSET_ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			actorMap[guid] = actor;

			return actor;
		}

		void Level::runGarbageCollector(EngineDevice& device) {
			if (actorKillQueue.size() != 0) {
				for (int i = 0; i < actorKillQueue.size(); i++) {
					SHARD3D_LOG("Destroying actor '{0}'", actorKillQueue.at(i).getTag());
					if (actorKillQueue.at(i).hasComponent<Components::MeshComponent>() || 
						actorKillQueue.at(i).hasComponent<Components::BillboardComponent>()) 
							vkDeviceWaitIdle(device.device());
					actorMap.erase(actorKillQueue.at(i).getGUID());
					registry.destroy(actorKillQueue.at(i));
				}
				actorKillQueue.clear();
				return;
			}
			if (actorReloadMeshQueue.size() != 0) {
				for (int i = 0; i < actorReloadMeshQueue.size(); i++) {
					vkDeviceWaitIdle(device.device());
					actorReloadMeshQueue.at(i).getComponent<Components::MeshComponent>().reapplyMesh(actorReloadMeshQueue.at(i).getComponent<Components::MeshComponent>().cacheFile);
				}
				actorReloadMeshQueue.clear();
				return;
			}
			if (actorKillMeshQueue.size() != 0) {
				for (int i = 0; i < actorKillMeshQueue.size(); i++) {
					vkDeviceWaitIdle(device.device());
					registry.remove<Components::MeshComponent>(actorKillMeshQueue.at(i));
				}
				actorKillMeshQueue.clear();
			}
			if (actorReloadTexQueue.size() != 0) {
				for (int i = 0; i < actorReloadTexQueue.size(); i++) {
					vkDeviceWaitIdle(device.device());
					actorReloadTexQueue.at(i).getComponent<Components::BillboardComponent>().reapplyTexture(actorReloadTexQueue.at(i).getComponent<Components::BillboardComponent>().cacheFile);
				}
				actorReloadTexQueue.clear();
				return;
			}
			if (actorKillTexQueue.size() != 0) {
				for (int i = 0; i < actorKillTexQueue.size(); i++) {
					vkDeviceWaitIdle(device.device());
					registry.remove<Components::BillboardComponent>(actorKillMeshQueue.at(i));
				}
				actorKillMeshQueue.clear();
			}
		}

		Actor Level::getActorFromGUID(GUID guid) {
			if (actorMap.find(guid) != actorMap.end()) {
				return { actorMap.at(guid), this };
			} else {
				SHARD3D_ERROR("Failed to find actor with guid '" + std::to_string(guid) + "'!");
				return { actorMap.at(1), this }; // dummy actor
			}
		}

		Actor Level::getActorFromTag(const std::string& tag) {
			auto guidView = registry.view<Components::TagComponent>();
			for (auto actor : guidView) {
				if (guidView.get<Components::TagComponent>(actor).tag == tag) {
					return Actor{ actor, this };
				}
			}
			SHARD3D_ERROR("Failed to find actor '" + tag + "'!");
			return Actor();
		}

		void Level::setPossessedCameraActor(Actor actor) {
			if (!actor.hasComponent<Components::CameraComponent>()) {
				SHARD3D_ERROR("Cannot possess an actor without a camera!!");
				return;
			}
			possessedCameraActorGUID = actor.getGUID();
		}

		void Level::setPossessedCameraActor(GUID guid) {
			if (!getActorFromGUID(guid).hasComponent<Components::CameraComponent>()) {
				SHARD3D_ERROR("Cannot possess an actor without a camera!!");
				return;
			}
			possessedCameraActorGUID = guid;
		}

		Actor Level::getPossessedCameraActor() {
			Actor actor = getActorFromGUID(possessedCameraActorGUID);
			if (actor.getGUID() != 1) return actor;
			SHARD3D_FATAL("No possessed camera found!!!!\nAttempted to find GUID: " + possessedCameraActorGUID);
		}

		 EngineCamera& Level::getPossessedCamera() {
			return getPossessedCameraActor().getComponent<Components::CameraComponent>().camera;
		 }
		 void Level::parentActor(Actor parent, Actor child){
			 child.parentHandle = parent.actorHandle;	
			// child.getTransform().parent_mat4 = &parent.getTransform().mat4();
		 }
#if ENSET_ALLOW_PREVIEW_CAMERA// ONLY FOR DEBUGGING PURPOSES
		 void Level::setPossessedPreviewCameraActor(Actor actor) {
			 if (!actor.hasComponent<Components::CameraComponent>()) {
				 SHARD3D_ERROR("Can't possess a non camera actor!!");
				 return;
			 }
			 possessedPreviewCameraActorGUID = actor.getGUID();
		 }

		 void Level::setPossessedPreviewCameraActor(GUID guid)  {
			 possessedPreviewCameraActorGUID = guid;
		 }

		 Actor Level::getPossessedPreviewCameraActor() {
			 auto guidView = registry.view<Components::GUIDComponent>();
			 for (auto actor : guidView) {
				 if (guidView.get<Components::GUIDComponent>(actor).id == possessedPreviewCameraActorGUID) {
					 //std::cout << "Using possessed: 0x" << &camView.get<Components::CameraComponent>(actor).camera << "\n";
					 return Actor{ actor, this };
				 }
			 }
			 SHARD3D_FATAL("No possessed camera found!!!!\nAttempted to find GUID: " + possessedPreviewCameraActorGUID);
		 }

		 EngineCamera& Level::getPossessedPreviewCamera() {
			 return getPossessedPreviewCameraActor().getComponent<Components::CameraComponent>().camera;
		 }
#endif
		void Level::begin() {
			SHARD3D_INFO("Level: Initializing scripts");
			simulationState = PlayState::Simulating;
			{
				registry.view<Components::CppScriptComponent>().each([&](auto actor, auto& csc) {
					if (!csc.Inst) {
						csc.Inst = csc.InstScript();
						csc.Inst->thisActor = Actor{ actor, this };
						csc.Inst->beginEvent();
					}
				});
			}
			{
				DynamicScriptEngine::runtimeStart(this);
				registry.view<Components::ScriptComponent>().each([&](auto actor, auto& scr) {
					DynamicScriptEngine::actorScript().beginEvent({ actor, this });
				});
			}
			{
				for (HUD* hud : Singleton::hudList) for (auto& element : hud->elements)
						DynamicScriptEngine::hudScript().begin(element.second.get());
			}
			SHARD3D_INFO("Beginning simulation");
		}

		void Level::tick(float dt) {
			// update scripts	
			{ // scoped because if tickevent does something else then it doesnt need to keep stack allocated memory
					registry.view<Components::CppScriptComponent>().each([&](auto actor, auto& csc) {
					csc.Inst->tickEvent(dt);
				});
			}	
			{ // Actor loop gets managed by the script engine
				DynamicScriptEngine::actorScript().tickEvent(dt);
			}
		}

		void Level::simulationStateCallback() {
			if (simulationState == PlayState::Paused) {
				_possessedCameraActorGUID = possessedCameraActorGUID;
				setPossessedCameraActor(0);
				auto audioView = registry.view<Components::AudioComponent>();
				for (auto actor : audioView) {
					audioView.get<Components::AudioComponent>(actor).pause();
				}
				wasPaused = true;
				SHARD3D_LOG("LEVEL: PAUSED");
			}
			if (simulationState == PlayState::Simulating || simulationState == PlayState::Playing) {
				setPossessedCameraActor(_possessedCameraActorGUID);
				auto audioView = registry.view<Components::AudioComponent>();
				for (auto actor : audioView) {
					audioView.get<Components::AudioComponent>(actor).resume();
				}
				wasPaused = false;
				SHARD3D_LOG("Level: Resumed");
			}
		}

		void Level::end() {
			SHARD3D_INFO("Ending Level Simulation");
			simulationState = PlayState::Stopped;
			{
				registry.view<Components::CppScriptComponent>().each([=](auto actor, auto& csc) {
					csc.Inst->endEvent();
				}); 
			}
			{
				DynamicScriptEngine::actorScript().endEvent();
			}
			{
				for (HUD* hud : Singleton::hudList) for (auto& element : hud->elements)
					DynamicScriptEngine::hudScript().end(element.second.get());
			}
			DynamicScriptEngine::runtimeStop();
			setPossessedCameraActor(0);
			SHARD3D_LOG("Reloading level");
			loadRegistryCapture = true;
		}

		void Level::killActor(Actor actor) {
			actorKillQueue.emplace_back(actor);
			if (simulationState == PlayState::Simulating) {
				if (!actor.hasComponent<Components::CppScriptComponent>()) goto next;
				{
					if (actor.getComponent<Components::CppScriptComponent>().Inst) {
						actor.getComponent<Components::CppScriptComponent>().Inst->killEvent();
						actor.getComponent<Components::CppScriptComponent>().killScript(&actor.getComponent<Components::CppScriptComponent>());
					}
				}	next:;
				if (!actor.hasComponent<Components::ScriptComponent>()) goto _next;
				{
					DynamicScriptEngine::actorScript().killEvent(actor);
				}	_next:;
			}
		}

		void Level::killMesh(Actor actor) {
			actorKillMeshQueue.emplace_back(actor);
		}

		void Level::killTexture(Actor actor) {
			actorKillTexQueue.emplace_back(actor);
		}

		void Level::reloadMesh(Actor actor) {
			actorReloadMeshQueue.emplace_back(actor);
		}

		void Level::reloadTexture(Actor actor) {
			actorReloadTexQueue.emplace_back(actor);
		}

		void Level::killEverything() {
			registry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, this };
				if (actor.isInvalid()) return;
				killActor(actor);
			});
		}
	}
}