#include "../s3dtpch.h"
#include "dynamic_script_engine_linker.hpp"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Shard3D {
	void DynamicScriptEngineLinker::registerLinker() {
		registerInternalCalls();
	}
	void DynamicScriptEngineLinker::registerInternalCalls() {
		_S3D_ICALL(Log);
		_S3D_ICALL(LogNoImpl);
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
}