#pragma once
#include <string>
namespace Shard3D {
	struct ScriptEngineData;
	class DynamicScriptEngine {
	public:	
		enum class Language {
			CSharp,
			VisualBasic
		};
		static void init();
		static void destroy();
		static void reloadAssembly(Language lang);
	private:
		inline static void _reloadAssembly(ScriptEngineData* scriptEngine, Language lang);
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