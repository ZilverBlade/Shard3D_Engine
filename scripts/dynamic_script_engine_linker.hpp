#pragma once
#include "../plugins/script_engine_plugin_link.h"

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
		void Log(MonoString* string, int severity);
		void LogNoImpl();
#pragma endregion

	
	
	}
}