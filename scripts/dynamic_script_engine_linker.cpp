#include "../s3dtpch.h"
#include "dynamic_script_engine_linker.hpp"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include "dynamic_script_engine.hpp"
#include "../wb3d/actor.hpp"

namespace Shard3D {
	struct _helper_container {
		std::vector<std::function<bool(wb3d::Actor)>> monoTypeComboRegistryAppenderHasComponent{};
		std::vector<std::function<void(wb3d::Actor)>> monoTypeComboRegistryAppenderAddComponent{};
		std::vector<std::function<void(wb3d::Actor)>> monoTypeComboRegistryAppenderRmvComponent{};
		std::vector<MonoTypeCombo*> monoTypeComboRegistryVector{};
	};
	static _helper_container* __h_ctr{};
	
	MonoTypeCombo::MonoTypeCombo(const std::string& type_builder) :
	_ctp(mono_reflection_type_from_name(const_cast<char*>(type_builder.data()), DynamicScriptEngine::getCoreAssemblyImage(0))),
	_vtp(mono_reflection_type_from_name(const_cast<char*>(type_builder.data()), DynamicScriptEngine::getCoreAssemblyImage(1))) {}

	void DynamicScriptEngineLinker::_sanity_check() {
		SHARD3D_LOG("SizeOf container {0}", __h_ctr->monoTypeComboRegistryVector.size());
	}

	template <typename/*...*/ Component>
	void registerComponent(){//([](){
		std::string_view type_name = typeid(Component).name();
		size_t pos = type_name.find_last_of(":");
		std::string_view struct_name = type_name.substr(pos + 1);
		std::string corrected_typename = fmt::format("Shard3D.Components.{}", struct_name);
		MonoTypeCombo* managedType = new MonoTypeCombo(corrected_typename);
		SHARD3D_LOG("SizeOf container {0}", __h_ctr->monoTypeComboRegistryVector.size());
		if (!*managedType) {
			SHARD3D_ERROR("Failed to find component {0}", corrected_typename);
			return;
		}

		__h_ctr->monoTypeComboRegistryVector.push_back(managedType);
		__h_ctr->monoTypeComboRegistryAppenderHasComponent.push_back([](wb3d::Actor actor) { return actor.hasComponent<Component>(); });
		__h_ctr->monoTypeComboRegistryAppenderAddComponent.push_back([](wb3d::Actor actor) { actor.addComponent<Component>(); });
		__h_ctr->monoTypeComboRegistryAppenderRmvComponent.push_back([](wb3d::Actor actor) { actor.killComponent<Component>(); });
		SHARD3D_LOG("Registered component: '{0}'", corrected_typename);
	}//(), ...);}

	void DynamicScriptEngineLinker::registerLinker() {
		registerInternalCalls();
		__h_ctr = new _helper_container();
		registerComponents();
		_sanity_check();
	}
	void DynamicScriptEngineLinker::registerInternalCalls() {
		_S3D_ICALL(Log);
		_S3D_ICALL(LogNoImpl);

		_S3D_ICALL(TransformComponent_SetTranslation);
		_S3D_ICALL(TransformComponent_GetTranslation);
		_S3D_ICALL(TransformComponent_SetRotation);
		_S3D_ICALL(TransformComponent_GetRotation);
		_S3D_ICALL(TransformComponent_SetScale);
		_S3D_ICALL(TransformComponent_GetScale);

		_S3D_ICALL(SpawnActor);
		_S3D_ICALL(Actor_HasComponent);
		_S3D_ICALL(Actor_AddComponent);
		_S3D_ICALL(Actor_RmvComponent);

		_S3D_ICALL(PointlightComponent_GetColor);
		_S3D_ICALL(PointlightComponent_SetColor);
		_S3D_ICALL(PointlightComponent_GetIntensity);
		_S3D_ICALL(PointlightComponent_SetIntensity);
		_S3D_ICALL(PointlightComponent_GetAttenuationFactor);
		_S3D_ICALL(PointlightComponent_SetAttenuationFactor);
		_S3D_ICALL(PointlightComponent_GetSpecularFactor);
		_S3D_ICALL(PointlightComponent_SetSpecularFactor);
		_S3D_ICALL(PointlightComponent_GetRadius);
		_S3D_ICALL(PointlightComponent_SetRadius);
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

	void TransformComponent_GetTranslation(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getTranslation();
	}
	void TransformComponent_SetTranslation(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setTranslation(*v);
	}
	void TransformComponent_GetRotation(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getRotation();
	}
	void TransformComponent_SetRotation(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setRotation(*v);
	}
	void TransformComponent_GetScale(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getScale();
	}
	void TransformComponent_SetScale(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setScale(*v);
	}
	void SpawnActor(uint64_t* actorID, MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);
		*actorID = DynamicScriptEngine::getContext()->createActor(text).getGUID();
		mono_free(t);
	}
	bool Actor_HasComponent(uint64_t actorID, MonoReflectionType* componentType, int lang) {
		wb3d::Actor actor = DynamicScriptEngine::getContext()->getActorFromGUID(actorID);
		MonoType* monoComponentType = mono_reflection_type_get_type(componentType);
		int i = 0;
		for (MonoTypeCombo* object : __h_ctr->monoTypeComboRegistryVector) {
			if ((!lang ? object->getCSharpType() : object->getVBasicType()) == monoComponentType)
				return __h_ctr->monoTypeComboRegistryAppenderHasComponent.at(i)(actor);
			i++;
		}
		return false;
	}
	void Actor_AddComponent(uint64_t actorID, MonoReflectionType* componentType, int lang) {
		wb3d::Actor actor = DynamicScriptEngine::getContext()->getActorFromGUID(actorID);
		MonoType* monoComponentType = mono_reflection_type_get_type(componentType);
		int i = 0;
		for (MonoTypeCombo* object : __h_ctr->monoTypeComboRegistryVector) {
			if ((!lang ? object->getCSharpType() : object->getVBasicType()) == monoComponentType)
				__h_ctr->monoTypeComboRegistryAppenderAddComponent.at(i)(actor);
			i++;
		}
	}
	void Actor_RmvComponent(uint64_t actorID, MonoReflectionType* componentType, int lang) {
		wb3d::Actor actor = DynamicScriptEngine::getContext()->getActorFromGUID(actorID);
		MonoType* monoComponentType = mono_reflection_type_get_type(componentType);
		int i = 0;
		for (MonoTypeCombo* object : __h_ctr->monoTypeComboRegistryVector) {
			if ((!lang ? object->getCSharpType() : object->getVBasicType()) == monoComponentType)
				__h_ctr->monoTypeComboRegistryAppenderRmvComponent.at(i)(actor);
			i++;
		}
	}

	void PointlightComponent_GetColor(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().color;
	}

	void PointlightComponent_SetColor(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().color = *v;
	}

	void PointlightComponent_GetIntensity(uint64_t actorID, float* v) {

	}

	void PointlightComponent_SetIntensity(uint64_t actorID, float* v) {

	}

	void PointlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v) {

	}

	void PointlightComponent_SetAttenuationFactor(uint64_t actorID, glm::vec3* v) {
		 
	}

	void PointlightComponent_GetSpecularFactor(uint64_t actorID, float* v) {

	}

	void PointlightComponent_SetSpecularFactor(uint64_t actorID, float* v) {

	}

	void PointlightComponent_GetRadius(uint64_t actorID, float* v) {

	}

	void PointlightComponent_SetRadius(uint64_t actorID, float* v) {

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