#pragma once
#include <string>
namespace Shard3D {
	struct ScriptEngineData;
	class DynamicScriptEngine {
	public:
		static void init();
		static void destroy();
	private:
		static void initMono(const char* lang, ScriptEngineData* scriptEngine);
		static void destroyMono(ScriptEngineData* scriptEngine);
	};

	namespace Components {	// Components must always be in these namespaces
		struct CSScriptComponent {
			std::string module;
			
		};
		struct VBScriptComponent {
			std::string module;
		};
	}
}