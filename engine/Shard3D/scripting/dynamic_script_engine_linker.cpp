#include "../s3dpch.h"
#include "dynamic_script_engine_linker.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include "dynamic_script_engine.h"
#include "../ecs.h"
#include "../core/ui/hudmgr.h"

namespace Shard3D {
	struct _helper_container {
		std::vector<std::function<bool(ECS::Actor)>> monoTypeComboRegistryAppenderHasComponent{};
		std::vector<std::function<void(ECS::Actor)>> monoTypeComboRegistryAppenderAddComponent{};
		std::vector<std::function<void(ECS::Actor)>> monoTypeComboRegistryAppenderRmvComponent{};
		std::vector<MonoTypeCombo*> monoTypeComboRegistryVector{};
	};
	static _helper_container* __h_ctr{};
	
	MonoTypeCombo::MonoTypeCombo(const std::string& type_builder) :
	_ctp(mono_reflection_type_from_name(const_cast<char*>(type_builder.data()), DynamicScriptEngine::getCoreAssemblyImage(0))),
	_vtp(mono_reflection_type_from_name(const_cast<char*>(type_builder.data()), DynamicScriptEngine::getCoreAssemblyImage(1))) {}

	template <typename Component>
	void registerComponent() {
		std::string_view type_name = typeid(Component).name();
		size_t pos = type_name.find_last_of(":");
		std::string_view struct_name = type_name.substr(pos + 1);
		std::string corrected_typename = fmt::format("Shard3D.Components.{}", struct_name);
		MonoTypeCombo* managedType = new MonoTypeCombo(corrected_typename);
		if (!*managedType) {
			SHARD3D_ERROR("Failed to find component {0}! Are both C# and VB assemblies up to date?", corrected_typename);
			return;
		}

		__h_ctr->monoTypeComboRegistryVector.push_back(managedType);
		__h_ctr->monoTypeComboRegistryAppenderHasComponent.push_back([](ECS::Actor actor) { return actor.hasComponent<Component>(); });
		__h_ctr->monoTypeComboRegistryAppenderAddComponent.push_back([](ECS::Actor actor) { actor.addComponent<Component>(); });
		__h_ctr->monoTypeComboRegistryAppenderRmvComponent.push_back([](ECS::Actor actor) { actor.killComponent<Component>(); });
		SHARD3D_LOG("Registered component: '{0}'", corrected_typename);
	}

	void DynamicScriptEngineLinker::registerLinker() {
		registerInternalCalls();
		__h_ctr = new _helper_container();
		registerComponents();
	}
	void DynamicScriptEngineLinker::registerInternalCalls() {
		_S3D_ICALL(Log);
		_S3D_ICALL(LogNoImpl);

		_S3D_ICALL(SceneManager_PossessCameraActor);

		_S3D_ICALL(SpawnActor);

		_S3D_ICALL(SceneManager_PossessCameraActor);

		_S3D_ICALL(SpawnActor);
		_S3D_ICALL(KillActor);
		_S3D_ICALL(Actor_HasComponent);
		_S3D_ICALL(Actor_AddComponent);
		_S3D_ICALL(Actor_RmvComponent);
		_S3D_ICALL(GetActorByTag);

		_S3D_ICALL(SceneManagerLoadLevel);
		_S3D_ICALL(SceneManagerLoadHUD);
		_S3D_ICALL(SceneManagerDestroyHUDLayer);

		_S3D_ICALL(TransformComponent_SetTranslation);
		_S3D_ICALL(TransformComponent_GetTranslation);
		_S3D_ICALL(TransformComponent_SetRotation);
		_S3D_ICALL(TransformComponent_GetRotation);
		_S3D_ICALL(TransformComponent_SetScale);
		_S3D_ICALL(TransformComponent_GetScale);

		_S3D_ICALL(CameraComponent_GetFOV);
		_S3D_ICALL(CameraComponent_SetFOV);
		_S3D_ICALL(CameraComponent_GetProjectionType);
		_S3D_ICALL(CameraComponent_SetProjectionType);
		_S3D_ICALL(CameraComponent_GetNearClip);
		_S3D_ICALL(CameraComponent_SetNearClip);
		_S3D_ICALL(CameraComponent_GetFarClip);
		_S3D_ICALL(CameraComponent_SetFarClip);

		_S3D_ICALL(AudioComponent_GetFile);
		_S3D_ICALL(AudioComponent_SetFile);
		_S3D_ICALL(AudioComponent_GetPropertiesPitch);
		_S3D_ICALL(AudioComponent_SetPropertiesPitch);
		_S3D_ICALL(AudioComponent_GetPropertiesVolume);
		_S3D_ICALL(AudioComponent_SetPropertiesVolume);
		_S3D_ICALL(AudioComponent_Play);
		_S3D_ICALL(AudioComponent_Stop);
		_S3D_ICALL(AudioComponent_Pause);
		_S3D_ICALL(AudioComponent_Resume);
		_S3D_ICALL(AudioComponent_Update);

		_S3D_ICALL(MeshComponent_GetFile);
		_S3D_ICALL(MeshComponent_SetFile);
		_S3D_ICALL(MeshComponent_Load);

		_S3D_ICALL(BillboardComponent_GetFile);
		_S3D_ICALL(BillboardComponent_SetFile);
		_S3D_ICALL(BillboardComponent_Load);

		_S3D_ICALL(PointlightComponent_GetIntensity);
		_S3D_ICALL(PointlightComponent_SetIntensity);
		_S3D_ICALL(PointlightComponent_GetAttenuationFactor);
		_S3D_ICALL(PointlightComponent_SetAttenuationFactor);
		_S3D_ICALL(PointlightComponent_GetSpecularFactor);
		_S3D_ICALL(PointlightComponent_SetSpecularFactor);
		_S3D_ICALL(PointlightComponent_GetRadius);
		_S3D_ICALL(PointlightComponent_SetRadius);

		_S3D_ICALL(SpotlightComponent_GetColor);
		_S3D_ICALL(SpotlightComponent_SetColor);
		_S3D_ICALL(SpotlightComponent_GetIntensity);
		_S3D_ICALL(SpotlightComponent_SetIntensity);
		_S3D_ICALL(SpotlightComponent_GetAttenuationFactor);
		_S3D_ICALL(SpotlightComponent_SetAttenuationFactor);
		_S3D_ICALL(SpotlightComponent_GetSpecularFactor);
		_S3D_ICALL(SpotlightComponent_SetSpecularFactor);
		_S3D_ICALL(SpotlightComponent_GetRadius);
		_S3D_ICALL(SpotlightComponent_SetRadius);
		_S3D_ICALL(SpotlightComponent_GetOuterAngle);
		_S3D_ICALL(SpotlightComponent_SetOuterAngle);
		_S3D_ICALL(SpotlightComponent_GetInnerAngle);
		_S3D_ICALL(SpotlightComponent_SetInnerAngle);

		_S3D_ICALL(DirectionalLightComponent_GetColor);
		_S3D_ICALL(DirectionalLightComponent_SetColor);
		_S3D_ICALL(DirectionalLightComponent_GetIntensity);
		_S3D_ICALL(DirectionalLightComponent_SetIntensity);
		_S3D_ICALL(DirectionalLightComponent_GetSpecularFactor);
		_S3D_ICALL(DirectionalLightComponent_SetSpecularFactor);
	}
	void DynamicScriptEngineLinker::registerComponents() {
		//registerComponent<Components::BlueprintComponent>();
		registerComponent<Components::TransformComponent>();
		registerComponent<Components::BillboardComponent>();
		registerComponent<Components::MeshComponent>();
		registerComponent<Components::AudioComponent>();
		registerComponent<Components::CameraComponent>();
		registerComponent<Components::DirectionalLightComponent>();
		registerComponent<Components::PointlightComponent>();
		registerComponent<Components::SpotlightComponent>();
		//registerComponent<Components::CppScriptComponent>();
		//registerComponent<Components::ScriptComponent>();
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
	void SceneManager_PossessCameraActor(uint64_t actorID) {
		DynamicScriptEngine::getContext()->setPossessedCameraActor(actorID);
	}
	void SpawnActor(uint64_t* actorID, MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);
		*actorID = DynamicScriptEngine::getContext()->createActor(text).getUUID();
		mono_free(t);
	}
	bool Actor_HasComponent(uint64_t actorID, MonoReflectionType* componentType, int lang) {
		ECS::Actor actor = DynamicScriptEngine::getContext()->getActorFromGUID(actorID);
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
		ECS::Actor actor = DynamicScriptEngine::getContext()->getActorFromGUID(actorID);
		MonoType* monoComponentType = mono_reflection_type_get_type(componentType);
		int i = 0;
		for (MonoTypeCombo* object : __h_ctr->monoTypeComboRegistryVector) {
			if ((!lang ? object->getCSharpType() : object->getVBasicType()) == monoComponentType)
				__h_ctr->monoTypeComboRegistryAppenderAddComponent.at(i)(actor);
			i++;
		}
	}
	void Actor_RmvComponent(uint64_t actorID, MonoReflectionType* componentType, int lang) {
		ECS::Actor actor = DynamicScriptEngine::getContext()->getActorFromGUID(actorID);
		MonoType* monoComponentType = mono_reflection_type_get_type(componentType);
		int i = 0;
		for (MonoTypeCombo* object : __h_ctr->monoTypeComboRegistryVector) {
			if ((!lang ? object->getCSharpType() : object->getVBasicType()) == monoComponentType)
				__h_ctr->monoTypeComboRegistryAppenderRmvComponent.at(i)(actor);
			i++;
		}
	}

	void KillActor(uint64_t actorID) {
		DynamicScriptEngine::getContext()->killActor(DynamicScriptEngine::getContext()->getActorFromGUID(actorID));
	}

	void GetActorByTag(uint64_t* actorID, MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);
		*actorID = DynamicScriptEngine::getContext()->getActorFromTag(text).getUUID();
		mono_free(t);
	}

	void TransformComponent_GetTranslation(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getTranslation();
	}
	void TransformComponent_SetTranslation(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setTranslation(*v);
	}
	void TransformComponent_GetRotation(uint64_t actorID, glm::vec3* v) {
		*v = glm::degrees((DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getRotation()));
	}
	void TransformComponent_SetRotation(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setRotation(glm::radians(*v));
	}
	void TransformComponent_GetScale(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().getScale();
	}
	void TransformComponent_SetScale(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getTransform().setScale(*v);
	}

	void CameraComponent_GetFOV(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().getFOV();
	}

	void CameraComponent_SetFOV(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().setFOV(*v);
	}

	void CameraComponent_GetProjectionType(uint64_t actorID, int* v) {
		*v = (int)DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().getProjectionType();
	}

	void CameraComponent_SetProjectionType(uint64_t actorID, int* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().setProjectionType((Components::CameraComponent::ProjectType)*v);
	}

	void CameraComponent_GetNearClip(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().getNearClip();
	}

	void CameraComponent_SetNearClip(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().setNearClip(*v);
	}

	void CameraComponent_GetFarClip(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().getFarClip();
	}

	void CameraComponent_SetFarClip(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::CameraComponent>().setFarClip(*v);
	}

	void AudioComponent_GetFile(uint64_t actorID, MonoString* string) {
		string = mono_string_new(DynamicScriptEngine::getDomain(), DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().file.c_str());
	}

	void AudioComponent_SetFile(uint64_t actorID, MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);

		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().file = text;
		mono_free(t);
	}

	void AudioComponent_GetPropertiesVolume(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().properties.volume;
	}

	void AudioComponent_SetPropertiesVolume(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().properties.volume = *v;
	}

	void AudioComponent_GetPropertiesPitch(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().properties.pitch;
	}

	void AudioComponent_SetPropertiesPitch(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().properties.pitch = *v;
	}

	void AudioComponent_Play(uint64_t actorID) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().play();
	}

	void AudioComponent_Pause(uint64_t actorID) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().pause();
	}

	void AudioComponent_Stop(uint64_t actorID) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().stop();
	}

	void AudioComponent_Resume(uint64_t actorID) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().resume();
	}

	void AudioComponent_Update(uint64_t actorID) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::AudioComponent>().update();
	}

	void MeshComponent_GetFile(uint64_t actorID, MonoString* string) {
	//	string = mono_string_new(DynamicScriptEngine::getDomain(), DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::MeshComponent>().file.c_str());
	}

	void MeshComponent_SetFile(uint64_t actorID, MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);

		//DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::MeshComponent>().cacheFile = text;
		mono_free(t);
	}

	void MeshComponent_Load(uint64_t actorID) {
		//auto& c = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::MeshComponent>();
		//c.reapplyMesh(c.cacheFile);
	}

	void BillboardComponent_GetFile(uint64_t actorID, MonoString* string) {
		//string = mono_string_new(DynamicScriptEngine::getDomain(), DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::BillboardComponent>().file.c_str());
	}

	void BillboardComponent_SetFile(uint64_t actorID, MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);

		//DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::BillboardComponent>().cacheFile = text;
		mono_free(t);
	}

	void BillboardComponent_Load(uint64_t actorID) {
		//auto& c = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::BillboardComponent>();
		//c.reapplyTexture(c.cacheFile);
	}

	void PointlightComponent_GetColor(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().color;
	}

	void PointlightComponent_SetColor(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().color = *v;
	}

	void PointlightComponent_GetIntensity(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().lightIntensity;
	}

	void PointlightComponent_SetIntensity(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().lightIntensity = *v;
	}

	void PointlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().attenuationMod;
	}

	void PointlightComponent_SetAttenuationFactor(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().attenuationMod = *v;
	}

	void PointlightComponent_GetSpecularFactor(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().specularMod;
	}

	void PointlightComponent_SetSpecularFactor(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::PointlightComponent>().specularMod = *v;
	}

	void PointlightComponent_GetRadius(uint64_t actorID, float* v) {
		SHARD3D_NOIMPL;
	}

	void PointlightComponent_SetRadius(uint64_t actorID, float* v) {
		SHARD3D_NOIMPL;
	}

	void SpotlightComponent_GetColor(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().color;
	}

	void SpotlightComponent_SetColor(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().color = *v;
	}

	void SpotlightComponent_GetIntensity(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().lightIntensity;
	}

	void SpotlightComponent_SetIntensity(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().lightIntensity = *v;
	}

	void SpotlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().attenuationMod;
	}

	void SpotlightComponent_SetAttenuationFactor(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().attenuationMod = *v;
	}

	void SpotlightComponent_GetSpecularFactor(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().specularMod;
	}

	void SpotlightComponent_SetSpecularFactor(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().specularMod = *v;
	}
	void SpotlightComponent_GetOuterAngle(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().outerAngle;
	}

	void SpotlightComponent_SetOuterAngle(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().outerAngle = *v;
	}

	void SpotlightComponent_GetInnerAngle(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().innerAngle;
	}

	void SpotlightComponent_SetInnerAngle(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::SpotlightComponent>().innerAngle = *v;
	}

	void SpotlightComponent_GetRadius(uint64_t actorID, float* v) {
		SHARD3D_NOIMPL;
	}

	void SpotlightComponent_SetRadius(uint64_t actorID, float* v) {
		SHARD3D_NOIMPL;
	}

	void DirectionalLightComponent_GetColor(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::DirectionalLightComponent>().color = *v;
	}

	void DirectionalLightComponent_SetColor(uint64_t actorID, glm::vec3* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::DirectionalLightComponent>().color = *v;
	}

	void DirectionalLightComponent_GetIntensity(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::DirectionalLightComponent>().lightIntensity;
	}

	void DirectionalLightComponent_SetIntensity(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::DirectionalLightComponent>().lightIntensity = *v;
	}
	void DirectionalLightComponent_GetSpecularFactor(uint64_t actorID, float* v) {
		*v = DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::DirectionalLightComponent>().specularMod;
	}

	void DirectionalLightComponent_SetSpecularFactor(uint64_t actorID, float* v) {
		DynamicScriptEngine::getContext()->getActorFromGUID(actorID).getComponent<Components::DirectionalLightComponent>().specularMod = *v;
	}
	void SceneManagerLoadLevel(MonoString* string) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);

		sPtr<ECS::Level> ptr(DynamicScriptEngine::getContext());

		ECS::LevelManager l_man = ECS::LevelManager(ptr);
		l_man.load(text, true);
		mono_free(t);
	}
	void SceneManagerLoadHUD(MonoString* string, int layer) {
		char* t = mono_string_to_utf8(string);
		std::string text(t);

		ECS::HUDManager h_man(DynamicScriptEngine::getHUDContext());
		h_man.load(text, layer, true);
		mono_free(t);
	}
	void SceneManagerDestroyHUDLayer(int layer) {
		DynamicScriptEngine::getHUDContext()->wipe(layer);
	}
}