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
			std::shared_ptr<Level> newLvl = std::make_shared<Level>();
			
			std::unordered_map<GUID, entt::entity> enttMap;

			auto& srcLvlRegistry = other->registry;
			auto& dstLvlRegistry = newLvl->registry;
			auto idView = srcLvlRegistry.view<Components::GUIDComponent>();
			for (auto e : idView) {
				GUID guid = srcLvlRegistry.get<Components::GUIDComponent>(e).id;
				Actor newActor = newLvl->createActorWithGUID(guid, srcLvlRegistry.get<Components::TagComponent>(e).tag);
				enttMap[guid] = (entt::entity)newActor;
			}

			copyComponent<Components::TransformComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::MeshComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::CameraComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::DirectionalLightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::PointlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::SpotlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::CppScriptComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);

			return newLvl;
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