#pragma once
#include "../plugins/script_engine_plugin_link.h"

extern "C" {
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
	typedef struct _MonoReflectionType MonoReflectionType;
	typedef struct _MonoType MonoType;
}

namespace Shard3D {
#define _S3D_ICALL(callName) mono_add_internal_call("Shard3D.Core.InternalCalls::"#callName, (void*)InternalScriptCalls::callName)
	struct MonoTypeCombo {
		MonoTypeCombo() = default;
		MonoTypeCombo(const std::string& type_builder);
		MonoType* getCSharpType() { return _ctp; }
		MonoType* getVBasicType() { return _vtp; }

		operator bool() { return _ctp && _vtp; }
	private:
		MonoType* _ctp{};
		MonoType* _vtp{};
	};
	class DynamicScriptEngineLinker {
	public:
		static void registerLinker();
	private:
		static void registerInternalCalls();
		static void registerComponents();
	};

	namespace InternalScriptCalls {
#pragma region Logging
		static void Log(MonoString* string, int severity);
		static void LogNoImpl();
#pragma endregion

#pragma region SceneManager
		static void SceneManager_PossessCameraActor(uint64_t actorID);

		static void SceneManagerLoadLevel(MonoString* string);
		static void SceneManagerLoadHUD(MonoString* string, int layer);
		static void SceneManagerDestroyHUDLayer(int layer);
#pragma endregion

		static bool IsKeyDown(int keyCode);
		static bool IsMouseButtonDown(int button);

#pragma region ECS
		static void SpawnActor(uint64_t* actorID, MonoString* string);
		static void KillActor(uint64_t actorID);
		static void GetActorByTag(uint64_t* actorID, MonoString* string);
		static bool Actor_HasComponent(uint64_t actorID, MonoReflectionType* componentType, int lang);
		static void Actor_AddComponent(uint64_t actorID, MonoReflectionType* componentType, int lang);
		static void Actor_RmvComponent(uint64_t actorID, MonoReflectionType* componentType, int lang);
#pragma endregion

#pragma region Components

#pragma region Transforms
		static void TransformComponent_GetTranslation(uint64_t actorID, glm::vec3* v);
		static void TransformComponent_SetTranslation(uint64_t actorID, glm::vec3* v);

		static void TransformComponent_GetRotation(uint64_t actorID, glm::vec3* v);
		static void TransformComponent_SetRotation(uint64_t actorID, glm::vec3* v);

		static void TransformComponent_GetScale(uint64_t actorID, glm::vec3* v);
		static void TransformComponent_SetScale(uint64_t actorID, glm::vec3* v);
#pragma endregion

#pragma region Camera
		static void CameraComponent_GetFOV(uint64_t actorID, float* v);
		static void CameraComponent_SetFOV(uint64_t actorID, float* v);

		static void CameraComponent_GetProjectionType(uint64_t actorID, int* v);
		static void CameraComponent_SetProjectionType(uint64_t actorID, int* v);

		static void CameraComponent_GetNearClip(uint64_t actorID, float* v);
		static void CameraComponent_SetNearClip(uint64_t actorID, float* v);

		static void CameraComponent_GetFarClip(uint64_t actorID, float* v);
		static void CameraComponent_SetFarClip(uint64_t actorID, float* v);
#pragma endregion

#pragma region Audio
		static void AudioComponent_GetFile(uint64_t actorID, MonoString* string);
		static void AudioComponent_SetFile(uint64_t actorID, MonoString* string);

		static void AudioComponent_GetPropertiesVolume(uint64_t actorID, float* v);
		static void AudioComponent_SetPropertiesVolume(uint64_t actorID, float* v);
		static void AudioComponent_GetPropertiesPitch(uint64_t actorID, float* v);
		static void AudioComponent_SetPropertiesPitch(uint64_t actorID, float* v);

		static void AudioComponent_Play(uint64_t actorID);
		static void AudioComponent_Pause(uint64_t actorID);
		static void AudioComponent_Stop(uint64_t actorID);
		static void AudioComponent_Resume(uint64_t actorID);
		static void AudioComponent_Update(uint64_t actorID);
#pragma endregion

#pragma region MeshComponent
		static void MeshComponent_GetFile(uint64_t actorID, MonoString* string);
		static void MeshComponent_SetFile(uint64_t actorID, MonoString* string);
		static void MeshComponent_Load(uint64_t actorID);
#pragma endregion

#pragma region BillboardComponent
		static void BillboardComponent_GetFile(uint64_t actorID, MonoString* string);
		static void BillboardComponent_SetFile(uint64_t actorID, MonoString* string);
		static void BillboardComponent_Load(uint64_t actorID);
#pragma endregion

#pragma region Pointlight
		static void PointlightComponent_GetColor(uint64_t actorID, glm::vec3* v);
		static void PointlightComponent_SetColor(uint64_t actorID, glm::vec3* v);

		static void PointlightComponent_GetIntensity(uint64_t actorID, float* v);
		static void PointlightComponent_SetIntensity(uint64_t actorID, float* v);

		static void PointlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v);
		static void PointlightComponent_SetAttenuationFactor(uint64_t actorID, glm::vec3* v);

		static void PointlightComponent_GetSpecularFactor(uint64_t actorID, float* v);
		static void PointlightComponent_SetSpecularFactor(uint64_t actorID, float* v);

		static void PointlightComponent_GetRadius(uint64_t actorID, float* v);
		static void PointlightComponent_SetRadius(uint64_t actorID, float* v);
#pragma endregion	

#pragma region Spotlight
		static void SpotlightComponent_GetColor(uint64_t actorID, glm::vec3* v);
		static void SpotlightComponent_SetColor(uint64_t actorID, glm::vec3* v);

		static void SpotlightComponent_GetIntensity(uint64_t actorID, float* v);
		static void SpotlightComponent_SetIntensity(uint64_t actorID, float* v);

		static void SpotlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v);
		static void SpotlightComponent_SetAttenuationFactor(uint64_t actorID, glm::vec3* v);

		static void SpotlightComponent_GetSpecularFactor(uint64_t actorID, float* v);
		static void SpotlightComponent_SetSpecularFactor(uint64_t actorID, float* v);

		static void SpotlightComponent_GetRadius(uint64_t actorID, float* v);
		static void SpotlightComponent_SetRadius(uint64_t actorID, float* v);

		static void SpotlightComponent_GetInnerAngle(uint64_t actorID, float* v);
		static void SpotlightComponent_SetInnerAngle(uint64_t actorID, float* v);

		static void SpotlightComponent_GetOuterAngle(uint64_t actorID, float* v);
		static void SpotlightComponent_SetOuterAngle(uint64_t actorID, float* v);
#pragma endregion

#pragma region DirectionalLight
		static void DirectionalLightComponent_GetColor(uint64_t actorID, glm::vec3* v);
		static void DirectionalLightComponent_SetColor(uint64_t actorID, glm::vec3* v);

		static void DirectionalLightComponent_GetIntensity(uint64_t actorID, float* v);
		static void DirectionalLightComponent_SetIntensity(uint64_t actorID, float* v);

		static void DirectionalLightComponent_GetSpecularFactor(uint64_t actorID, float* v);
		static void DirectionalLightComponent_SetSpecularFactor(uint64_t actorID, float* v);
#pragma endregion

#pragma endregion
	}
}