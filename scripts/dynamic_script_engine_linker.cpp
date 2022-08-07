#include "../s3dtpch.h"
#include "dynamic_script_engine_linker.hpp"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "dynamic_script_engine.hpp"
#include "../wb3d/actor.hpp"
#include "../wb3d/levelmgr.hpp"
#include "../wb3d/hudmgr.hpp"

namespace Shard3D {
	void DynamicScriptEngineLinker::registerLinker() {
		registerInternalCalls();
	}
	void DynamicScriptEngineLinker::registerInternalCalls() {
		_S3D_ICALL(Log);
		_S3D_ICALL(LogNoImpl);
		_S3D_ICALL(SetTranslation);
		_S3D_ICALL(GetTranslation);
		_S3D_ICALL(SetRotation);
		_S3D_ICALL(GetRotation);
		_S3D_ICALL(SetScale);
		_S3D_ICALL(GetScale);
		_S3D_ICALL(SpawnActor);
		_S3D_ICALL(ActorAddComponent);
		_S3D_ICALL(ActorRmvComponent);
		_S3D_ICALL(KillActor);
		_S3D_ICALL(SceneManagerLoadLevel);
		_S3D_ICALL(SceneManagerLoadHUD);
		_S3D_ICALL(SceneManagerDestroyHUDLayer);
	}
}
namespace Shard3D::InternalScriptCalls {
	void Log(MonoString* string, int severity) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);
		if (severity == 0) {
			SHARD3D_LOG(text);
		} else if (severity == 1) {
			SHARD3D_INFO(text);
		} else if (severity == 2) {
			SHARD3D_WARN(text);
		} else if (severity == 3) {
			SHARD3D_ERROR(text);
		} else {
			SHARD3D_ERROR("Internal call logged with invalid severity");
		}
		mono_free(t);
	}
	void LogNoImpl() {
		SHARD3D_NOIMPL;
	}

	void GetTranslation(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getTranslation();
	}
	void SetTranslation(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setTranslation(*v);
	}
	void GetRotation(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getRotation();
	}
	void SetRotation(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setRotation(*v);
	}
	void GetScale(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getScale();
	}
	void SetScale(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setScale(*v);
	}
	void SpawnActor(uint64_t* actorID, MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);
		*actorID = DynamicScriptEngine::getContext()->createActor(text).getGUID();
		mono_free(t);
	}
	void KillActor(uint64_t actorID) {
		DynamicScriptEngine::getContext()->killActor(DynamicScriptEngine::getContext()->getActorFromGUID(actorID));
	}
	void ActorAddComponent(uint64_t actorID, Components::ComponentsList component) {
		switch (component) {
			case Components::ComponentsList::AudioComponent:
				DynamicScriptEngine::getContext()->getActorFromGUID(actorID).addComponent<Components::AudioComponent>();
				break;
			case Components::ComponentsList::BillboardComponent:
				DynamicScriptEngine::getContext()->getActorFromGUID(actorID).addComponent<Components::BillboardComponent>();
				break;
			case Components::ComponentsList::CameraComponent:
				DynamicScriptEngine::getContext()->getActorFromGUID(actorID).addComponent<Components::CameraComponent>();
				break;
			case Components::ComponentsList::MeshComponent:
				DynamicScriptEngine::getContext()->getActorFromGUID(actorID).addComponent<Components::MeshComponent>();
				break;
			case Components::ComponentsList::PointlightComponent:
				DynamicScriptEngine::getContext()->getActorFromGUID(actorID).addComponent<Components::PointlightComponent>();
				break;
			case Components::ComponentsList::SpotlightComponent:
				DynamicScriptEngine::getContext()->getActorFromGUID(actorID).addComponent<Components::SpotlightComponent>();
				break;
			case Components::ComponentsList::DirectionalLightComponent:
				DynamicScriptEngine::getContext()->getActorFromGUID(actorID).addComponent<Components::DirectionalLightComponent>();
				break;
			default:
				SHARD3D_ERROR("Component provided does not exist!");
				break;
		}
	}
	void ActorRmvComponent(uint64_t actorID, Components::ComponentsList component) {
		switch (component) {
		case Components::ComponentsList::AudioComponent:
			DynamicScriptEngine::getContext()->getActorFromGUID(actorID).killComponent<Components::AudioComponent>();
			break;
		case Components::ComponentsList::BillboardComponent:
			DynamicScriptEngine::getContext()->getActorFromGUID(actorID).killComponent<Components::BillboardComponent>();
			break;
		case Components::ComponentsList::CameraComponent:
			DynamicScriptEngine::getContext()->getActorFromGUID(actorID).killComponent<Components::CameraComponent>();
			break;
		case Components::ComponentsList::MeshComponent:
			DynamicScriptEngine::getContext()->getActorFromGUID(actorID).killComponent<Components::MeshComponent>();
			break;
		case Components::ComponentsList::PointlightComponent:
			DynamicScriptEngine::getContext()->getActorFromGUID(actorID).killComponent<Components::PointlightComponent>();
			break;
		case Components::ComponentsList::SpotlightComponent:
			DynamicScriptEngine::getContext()->getActorFromGUID(actorID).killComponent<Components::SpotlightComponent>();
			break;
		case Components::ComponentsList::DirectionalLightComponent:
			DynamicScriptEngine::getContext()->getActorFromGUID(actorID).killComponent<Components::DirectionalLightComponent>();
			break;
		default:
			SHARD3D_ERROR("Component provided does not exist!");
			break;
		}
	}
	void SceneManagerLoadLevel(MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);

		std::shared_ptr<wb3d::Level> ptr(DynamicScriptEngine::getContext());

		wb3d::LevelManager l_man = wb3d::LevelManager(ptr);
		l_man.load(text, true);
		mono_free(t);
	}
	void SceneManagerLoadHUD(MonoString* string, int layer) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);

		wb3d::HUDManager h_man(DynamicScriptEngine::getHUDContext());
		h_man.load(text, layer, true);
		mono_free(t);
	}
	void SceneManagerDestroyHUDLayer(int layer) {
		DynamicScriptEngine::getHUDContext()->wipe(layer);
	}
}