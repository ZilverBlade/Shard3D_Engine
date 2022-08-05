#pragma once
#include "../plugins/script_engine_plugin_link.h"
#include "../components.hpp"
#include "../wb3d/actor.hpp"
#include <unordered_map>
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
		MonoTypeCombo(MonoTypeCombo& other) = default;

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

#pragma region Transforms
		static void GetTranslation(uint64_t actorID, glm::vec3* v);
		static void SetTranslation(uint64_t actorID, glm::vec3* v);

		static void GetRotation(uint64_t actorID, glm::vec3* v);
		static void SetRotation(uint64_t actorID, glm::vec3* v);

		static void GetScale(uint64_t actorID, glm::vec3* v);
		static void SetScale(uint64_t actorID, glm::vec3* v);
#pragma endregion

#pragma region ECS
		static void SpawnActor(uint64_t* actorID, MonoString* string);
		static bool Actor_HasComponent(uint64_t actorID, MonoReflectionType* componentType);
		static void ActorAddComponent(uint64_t actorID, Components::ComponentsList component);
		static void ActorRmvComponent(uint64_t actorID, Components::ComponentsList component);
#pragma endregion

#pragma region Components
#pragma region Pointlight
		static void PointlightComponent_GetColor(uint64_t actorID, glm::vec3* v);
		static void PointlightComponent_GetIntensity(uint64_t actorID, float* v);
		static void PointlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v);
		static void PointlightComponent_GetSpecularFactor(uint64_t actorID, float* v);
		static void PointlightComponent_GetRadius(uint64_t actorID, float* v);
#pragma endregion	
#pragma region SpotlightComponent
		static void SpotlightComponent_GetColor(uint64_t actorID, glm::vec3* v);
		static void SpotlightComponent_GetIntensity(uint64_t actorID, float* v);
		static void SpotlightComponent_GetAttenuationFactor(uint64_t actorID, glm::vec3* v);
		static void SpotlightComponent_GetSpecularFactor(uint64_t actorID, float* v);
		static void SpotlightComponent_GetRadius(uint64_t actorID, float* v);
		static void SpotlightComponent_GetInnerAngle(uint64_t actorID, float* v);
		static void SpotlightComponent_GetOuterAngle(uint64_t actorID, float* v);
#pragma endregion
#pragma endregion
	}
}