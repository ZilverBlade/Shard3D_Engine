#include "../s3dtpch.h"
#include "dynamic_script_engine_linker.hpp"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "dynamic_script_engine.hpp"
#include "../wb3d/actor.hpp"

namespace Shard3D {
	static std::unordered_map< std::string, MonoTypeCombo*> monoTypeComboRegistry;
	static std::unordered_map<MonoTypeCombo*, std::function<bool(wb3d::Actor)>> actorHasComponentFuncs;

	MonoTypeCombo::MonoTypeCombo(const std::string& type_builder) :
	_ctp(mono_reflection_type_from_name(const_cast<char*>(type_builder.data()), DynamicScriptEngine::getCoreAssemblyImage(0))),
	_vtp(mono_reflection_type_from_name(const_cast<char*>(type_builder.data()), DynamicScriptEngine::getCoreAssemblyImage(1))) 
	{}

	template <typename Component>
	static void registerComponent() {
	//	([]() {
			std::string_view type_name = typeid(Component).name();
			size_t pos = type_name.find_last_of(":");
			std::string_view struct_name = type_name.substr(pos + 1);
			std::string corrected_typename = fmt::format("Shard3D.Components.{}", struct_name);
			MonoTypeCombo* managedType{};
			managedType = new MonoTypeCombo(corrected_typename);

			if (!*managedType) {
				SHARD3D_ERROR("Failed to find component {0}", corrected_typename);
					return;
			}
			monoTypeComboRegistry[corrected_typename] = managedType;
			actorHasComponentFuncs[managedType] = [](wb3d::Actor actor) { return actor.hasComponent<Component>(); };
			SHARD3D_LOG("Registered component: '{0}'", corrected_typename);
	//	}(), ...);
	}


	void DynamicScriptEngineLinker::registerLinker() {
		registerInternalCalls();
		//registerComponents();
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
	}
	void DynamicScriptEngineLinker::registerComponents() {
		registerComponent<Components::BlueprintComponent>();
		registerComponent<Components::TransformComponent>();
		registerComponent<Components::BillboardComponent>();
		registerComponent<Components::MeshComponent>();
		registerComponent<Components::AudioComponent>();
		registerComponent<Components::CameraComponent>();
		registerComponent<Components::DirectionalLightComponent>();
		registerComponent<Components::PointlightComponent>();
		registerComponent<Components::SpotlightComponent>();
		registerComponent<Components::CppScriptComponent>();
		registerComponent<Components::ScriptComponent>();
	}
	
}
namespace Shard3D::InternalScriptCalls {
	void Log(MonoString* string, int severity) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);
		if (severity == 0) {
			SHARD3D_LOG(text);
		}
		else if (severity == 1) {
			SHARD3D_INFO(text);
		}
		else if (severity == 2) {
			SHARD3D_WARN(text);
		}
		else if (severity == 3) {
			SHARD3D_ERROR(text);
		}
		else {
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
	bool Actor_HasComponent(uint64_t actorID, MonoReflectionType* componentType) {
		wb3d::Actor actor = DynamicScriptEngine::getContext()->getActorFromGUID(actorID);
		MonoType* monoComponentType = mono_reflection_type_get_type(componentType);

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

	void PointlightComponent_GetColor(uint64_t actorID, glm::vec3* v) {

	}

	void PointlightComponent_GetIntensity(uint64_t actorID, float* v) {

	}

	void PointlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v) {

	}

	void PointlightComponent_GetSpecularFactor(uint64_t actorID, float* v) {

	}

	void PointlightComponent_GetRadius(uint64_t actorID, float* v) {

	}

	void SpotlightComponent_GetColor(uint64_t actorID, glm::vec3* v) {

	}

	void SpotlightComponent_GetIntensity(uint64_t actorID, float* v) {

	}

	void SpotlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v) {

	}

	void SpotlightComponent_GetSpecularFactor(uint64_t actorID, float* v) {

	}

	void SpotlightComponent_GetRadius(uint64_t actorID, float* v) {

	}

	void SpotlightComponent_GetInnerAngle(uint64_t actorID, float* v) {

	}

	void SpotlightComponent_GetOuterAngle(uint64_t actorID, float* v) {

	}

}