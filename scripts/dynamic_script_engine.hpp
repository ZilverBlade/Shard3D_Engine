#pragma once
#include <string>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace Shard3D {
	struct ScriptEngineData;
	class DynamicScriptClass;
	namespace wb3d {
		class Level;
		class Actor;
	}
	struct HUDElement;
	class HUDContainer;
	class DynamicScriptEngine {
	public:	
		enum class ScriptLanguage {
			CSharp,
			VisualBasic
		};
		struct _a {
			void beginEvent(wb3d::Actor __a);
			void endEvent();
			void tickEvent(float __dt);

			void spawnEvent(wb3d::Actor actor);
			void killEvent(wb3d::Actor actor);
		};
		struct _h {
			void begin(HUDElement* __h);	// special use case, doesnt call any events, just initialises
			void end(HUDElement* __h);		// special use case, doesnt call any events, just destroys
			void hoverEvent(HUDElement* __h, float __dt);
			void pressEvent(HUDElement* __h, float __dt);
			void clickEvent(HUDElement* __h);
		};
		static void init();
		static void destroy();
		static void reloadAssembly(ScriptLanguage lang);
		static std::unordered_map<std::string, std::shared_ptr<DynamicScriptClass>> getActorClasses(int lang);
		static void runtimeStart(wb3d::Level* level);
		static void runtimeStop();
		static void setHUDContext(HUDContainer* container);
		inline static bool doesClassExist(const std::string& fullClassName, int lang);
		inline static bool doesHUDClassExist(const std::string& fullClassName, int lang);
		static wb3d::Level* getContext();
		static HUDContainer* getHUDContext();
		static _a actorScript() { return _a(); }
		static _h hudScript() { return _h(); }
	private:
		static void _reloadAssembly(ScriptEngineData* scriptEngine, ScriptLanguage lang);
		static void destroyMono(ScriptEngineData* scriptEngine);

		static MonoObject* instClass(MonoClass* monoClass, int lang);
		static void loadAssemblyClasses(ScriptEngineData* data);
		friend class DynamicScriptClass;
	};

	class DynamicScriptClass {
	public:
		DynamicScriptClass() = default;
		DynamicScriptClass(const std::string& c_ns, const std::string& c_n, int _lang);

		MonoObject* inst();
		MonoMethod* getMethod(const std::string& name, int parameterCount);
		MonoObject* invokeMethod(MonoObject* inst, MonoMethod* method, void** params = nullptr);
	private:
		std::string classNamespace, className;
		int lang = 0;
		MonoClass* monoClass = nullptr;
	};
	class DynamicScriptInstance {
		struct ScriptEvents {
			ScriptEvents() = default;
			ScriptEvents(std::shared_ptr<DynamicScriptClass> ptr, MonoObject* i);
			void beginEvent();
			void endEvent();
			void tickEvent(float dt);

			void spawnEvent();
			void killEvent();
			
			MonoMethod* beginEventMethod{};
			MonoMethod* endEventMethod{};
			MonoMethod* tickEventMethod{};
			MonoMethod* spawnEventMethod{};
			MonoMethod* killEventMethod{};

			std::shared_ptr<DynamicScriptClass> s_c;
			MonoObject* _i{};
		};
		ScriptEvents scriptEvents;
	public:	
		DynamicScriptInstance(std::shared_ptr<DynamicScriptClass> s_class, int lang, wb3d::Actor actor);
		ScriptEvents invokeEvent();
	private:
		std::shared_ptr<DynamicScriptClass> scriptClass;

		MonoMethod* constructor{};
		MonoObject* instance{};		
	};
	class HUDScriptInstance {
		struct ScriptEvents {
			ScriptEvents() = default;
			ScriptEvents(std::shared_ptr<DynamicScriptClass> ptr, MonoObject* i);
			void hoverEvent(float dt);
			void pressEvent(float dt);
			void clickEvent();

			MonoMethod* hoverEventMethod{};
			MonoMethod* pressEventMethod{};
			MonoMethod* clickEventMethod{};

			std::shared_ptr<DynamicScriptClass> s_c;
			MonoObject* _i{};
		};
		ScriptEvents scriptEvents;
	public:
		HUDScriptInstance(std::shared_ptr<DynamicScriptClass> s_class, int lang, HUDElement* element);
		ScriptEvents invokeEvent();
	private:
		std::shared_ptr<DynamicScriptClass> scriptClass;

		MonoMethod* constructor{};
		MonoObject* instance{};
	};
	namespace Components {
		struct ScriptComponent {
			std::string name = "Example";
			int lang = 0; // C# = 0, VB = 1
			ScriptComponent() = default;
			ScriptComponent(const ScriptComponent&) = default;
		};
	}
}