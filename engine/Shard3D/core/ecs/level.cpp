#include "../../s3dpch.h"
#include "level.h"
#include "sactor.h" // also includes "actor.h"

#include "levelmgr.h"

#include "../asset/assetmgr.h"
#include "../../scripting/script_engine.h"
#include "../ui/hud.h"
#include "../../workarounds.h"
#include "../../systems/computational/physics_system.h"
#include "../../systems/handlers/render_handler.h"

namespace Shard3D {
	namespace ECS {
		Level::Level(const std::string& lvlName, bool makeSystem) : name(lvlName) {
			if (!makeSystem) return;
			SHARD3D_INFO("Loading editor camera actor");
			ECS::Actor editor_cameraActor = createActorWithUUID(0, "Editor Camera Actor (SYSTEM RESERVED)");
			editor_cameraActor.addComponent<Components::CameraComponent>();
			editor_cameraActor.getComponent<Components::CameraComponent>().postProcessMaterials.emplace_back(ResourceHandler::retrievePPOMaterial(AssetID("assets/_engine/mat/ppo/hdr_vfx.s3dasset")).get(), AssetID("assets/_engine/mat/ppo/hdr_vfx.s3dasset"));
			editor_cameraActor.getComponent<Components::CameraComponent>().postProcessMaterials.emplace_back(ResourceHandler::retrievePPOMaterial(AssetID("assets/_engine/mat/ppo/bloom_vfx.s3dasset")).get(), AssetID("assets/_engine/mat/ppo/bloom_vfx.s3dasset"));

			setPossessedCameraActor(editor_cameraActor);
			editor_cameraActor.getComponent<Components::TransformComponent>().setTranslation(glm::vec3(0.f, -1.f, 1.f));
			SHARD3D_INFO("Loading dummy actor");
			ECS::Actor dummy = createActorWithUUID(1, "Dummy Actor (SYSTEM RESERVED)");

		}
		Level::~Level() {
			SHARD3D_INFO("Destroying level");
			registry.clear();
		}

		template<typename Component>
		static void copyComponent(entt::registry& dst, entt::registry& src, hashMap<UUID, entt::entity>& map) {
			auto view = src.view<Component>();
			for (auto e : view) {
				UUID guid = src.get<Components::UUIDComponent>(e).getID();

				if (map.find(guid) == map.end()) SHARD3D_FATAL("enttMap.find(guid) == enttMap.end()");

				entt::entity dstEnttID = map.at(guid);

				auto& component = src.get<Component>(e);

				dst.emplace_or_replace<Component>(dstEnttID, component);
			}
		}

		void Level::shallowCopy(sPtr<Level>& this_, sPtr<Level>& other) {
			this_->possessedCameraActorGUID = other->getPossessedCameraActor().getUUID();
			this_->currentpath = other->currentpath;
			this_->name = other->name;
			this_->physicsSystemPtr = other->physicsSystemPtr;
		}

		sPtr<Level> Level::copy(sPtr<Level> other) {
			sPtr<Level> newLvl = make_sPtr<Level>(other->name, false);
			hashMap<UUID, entt::entity> enttMap;

			auto& srcLvlRegistry = other->registry;
			auto& dstLvlRegistry = newLvl->registry;

			auto idView = srcLvlRegistry.view<Components::UUIDComponent>();
			for (auto it = idView.begin(); it != idView.end(); ++it) {
				UUID guid = srcLvlRegistry.get<Components::UUIDComponent>(idView[it.index()]).getID();
				Actor newActor = newLvl->createActorWithUUID(guid, srcLvlRegistry.get<Components::TagComponent>(idView[it.index()]).tag);
				enttMap[guid] = newActor.actorHandle;
			}

			copyComponent<Components::TransformComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::BillboardComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::Mesh3DComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::AudioComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::CameraComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::DirectionalLightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::PointlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::SpotlightComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::ScriptComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::Rigidbody3DComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);

			// Secret components
			copyComponent<Components::CppScriptComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);
			copyComponent<Components::RelationshipComponent>(dstLvlRegistry, srcLvlRegistry, enttMap);

			shallowCopy(newLvl, other);

			RenderHandler::clear();
			for (auto& [uuid, handle] : enttMap) {
				Actor actor = Actor(handle, newLvl.get());
				if (actor.hasComponent<Components::Mesh3DComponent>()) RenderHandler::addActorToRenderList(actor);
			}

			return newLvl;
		}

		Actor Level::createActor(const std::string& name) {
			return createActorWithUUID(UUID(), name);
		}

		Actor Level::createActorWithUUID(UUID guid, const std::string& name) {
			SHARD3D_ASSERT(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { registry.create(), this };
			actor.addComponent<Components::UUIDComponent>(guid);
			actor.addComponent<Components::TagComponent>().tag = name;
			actor.addComponent<Components::TransformComponent>();

			actorMap[guid] = actor;

			return actor;
		}

		void Level::runGarbageCollector(EngineDevice& device) {
			if (actorKillQueue.size() != 0) {
				for (auto& actor : actorKillQueue) {
					SHARD3D_LOG("Destroying actor '{0}'", actor.getTag());
					UUID uuid = actor.getUUID();
					actorMap.erase(uuid);
					for (SurfaceMaterialClassOptionsFlags class_ : RenderHandler::getRenderUsingClasses(uuid)) {
						RenderHandler::rmvFromSurfaceMaterialRenderingList(uuid, class_);
					}
					registry.destroy(actor.actorHandle);
				}
				actorKillQueue.clear();
				return;
			}
			rebuildTransforms();
		}

		Actor Level::getActorFromUUID(UUID guid) {
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
			for (auto& instance : actor.getComponent<Components::CameraComponent>().postProcessMaterials)
				instance.updateBuffers();
			possessedCameraActorGUID = actor.getUUID();
		}

		void Level::setPossessedCameraActor(UUID guid) {
			if (!getActorFromUUID(guid).hasComponent<Components::CameraComponent>()) {
				SHARD3D_ERROR("Cannot possess an actor without a camera!!");
				return;
			}
			for (auto& instance : getActorFromUUID(guid).getComponent<Components::CameraComponent>().postProcessMaterials)
				instance.updateBuffers();	
			possessedCameraActorGUID = guid;
		}

		Actor Level::getPossessedCameraActor() {
			Actor actor = getActorFromUUID(possessedCameraActorGUID);
			if (actor.getUUID() != 1) return actor;
			SHARD3D_FATAL("No possessed camera found!!!!\nAttempted to find UUID: " + possessedCameraActorGUID);
		}

		EngineCamera& Level::getPossessedCamera() {
			return getPossessedCameraActor().getComponent<Components::CameraComponent>().camera;
		}
		void Level::parentActor(Actor child, Actor parent){
			SHARD3D_ASSERT(child != parent && "Cannot parent actor to itself!");
			child.addComponent<Components::RelationshipComponent>();
			parent.addComponent<Components::RelationshipComponent>();
			child.getComponent<Components::RelationshipComponent>().parentActor = parent.actorHandle;
			parent.getComponent<Components::RelationshipComponent>().childActors.push_back(child.actorHandle);

			SHARD3D_LOG("Parented actor {0} to {1}", parent.getUUID(), child.getUUID());
		}

		
		void Level::begin() {
			SHARD3D_ASSERT(physicsSystemPtr != nullptr && "Physics system not set!");
			SHARD3D_INFO("Level: Initializing scripts");
			simulationState = PlayState::Playing;
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
				ScriptEngine::runtimeStart(this);
				registry.view<Components::ScriptComponent>().each([&](auto actor, auto& scr) {
					ScriptEngine::actorScript().beginEvent({ actor, this });
				});
			}
			{
				for (HUD* hud : TEMPORARY::hudList) for (auto& element : hud->elements)
					ScriptEngine::hudScript().begin(element.second.get());
			}
		//	physicsSystemPtr->begin(this);
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
				ScriptEngine::actorScript().tickEvent(dt);
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
			if (simulationState == PlayState::Playing) {
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
				ScriptEngine::actorScript().endEvent();
			}
			{
				for (HUD* hud : TEMPORARY::hudList) for (auto& element : hud->elements)
					ScriptEngine::hudScript().end(element.second.get());
			}
			//physicsSystemPtr->end(this);
			ScriptEngine::runtimeStop();
			setPossessedCameraActor(0);
			SHARD3D_LOG("Reloading level");
		}

		void Level::killActor(Actor actor) {
			actorKillQueue.emplace_back(actor);
			if (simulationState == PlayState::Playing) {
				if (!actor.hasComponent<Components::CppScriptComponent>()) goto next;
				{
					if (actor.getComponent<Components::CppScriptComponent>().Inst) {
						actor.getComponent<Components::CppScriptComponent>().Inst->killEvent();
						actor.getComponent<Components::CppScriptComponent>().killScript(&actor.getComponent<Components::CppScriptComponent>());
					}
				}	next:;
				if (!actor.hasComponent<Components::ScriptComponent>()) goto _next;
				{
					ScriptEngine::actorScript().killEvent(actor);
				}	_next:;
			}

			if (actor.hasComponent<Components::RelationshipComponent>()) {
				if (actor.getComponent<Components::RelationshipComponent>().parentActor != entt::null) {
					std::vector<entt::entity>& childVector = Actor(actor.getComponent<Components::RelationshipComponent>().parentActor, this)
						.getComponent<Components::RelationshipComponent>().childActors;
					std::vector<entt::entity>::iterator q = childVector.begin();	
					for (int i = 0; i < childVector.size(); i++) {
						if (childVector.at(i) == actor) break;
						q++;
					}
					childVector.erase(q);
				}
				for (auto& child : actor.getComponent<Components::RelationshipComponent>().childActors) {
					killActor({ child, this });
				}
			}
		}

		void Level::rebuildTransforms() {
			auto view = registry.view<Components::TransformComponent>();
			for (auto obj : view) {
				ECS::Actor actor = { obj, this };
				if (!actor.getTransform().dirty) continue;
				if (actor.hasComponent<Components::RelationshipComponent>()) {
					rebuildRelations(actor);
					continue;
				}
				// regular actors with no relations get calculated normally if dirty
				actor.getTransform().transformMatrix = actor.getTransform().calculateMat4();
				actor.getTransform().normalMatrix = actor.getTransform().calculateNormalMatrix();
				actor.getTransform().dirty = false;
			}
		}

		void Level::rebuildRelations(Actor child) {
			glm::mat4 parentMatrix{ 1.f };
			glm::mat3 parentNormal{ 1.f };
			if (child.getComponent<Components::RelationshipComponent>().parentActor != entt::null) {
				Actor parentActor = { child.getComponent<Components::RelationshipComponent>().parentActor, this };
				parentMatrix = parentActor.getTransform().transformMatrix;
				parentNormal = parentActor.getTransform().normalMatrix;
			}
			child.getTransform().transformMatrix = parentMatrix * child.getTransform().calculateMat4();
			child.getTransform().normalMatrix = parentNormal * child.getTransform().calculateNormalMatrix();
			child.getTransform().dirty = false;
			for (auto& subchild : child.getComponent<Components::RelationshipComponent>().childActors) {
				rebuildRelations({ subchild, this });
			}
		}


		void Level::killEverything() {
			registry.each([&](auto actorGUID) { ECS::Actor actor = { actorGUID, this };
				if (actor.isInvalid()) return;
				actorKillQueue.emplace_back(actor);
			});
		}
	}
}