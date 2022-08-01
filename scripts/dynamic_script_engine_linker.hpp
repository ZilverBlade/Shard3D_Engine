#pragma once
#include "../plugins/script_engine_plugin_link.h"
#include "../components.hpp"
extern "C" {
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
	typedef struct _MonoReflectionType MonoReflectionType;
}

namespace Shard3D {
#define _S3D_ICALL(callName) mono_add_internal_call("Shard3D.Core.InternalCalls::"#callName, (void*)InternalScriptCalls::callName)

	class DynamicScriptEngineLinker {
	public:
		static void registerLinker();
	private:
		static void registerInternalCalls();
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
		static void ActorAddComponent(uint64_t actorID, Components::ComponentsList component);
		static void ActorRmvComponent(uint64_t actorID, Components::ComponentsList component);
#pragma endregion
	
	}
}