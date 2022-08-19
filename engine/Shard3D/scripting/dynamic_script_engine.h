#pragma once
#include <string>
#include "../s3dstd.h"
extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoDomain MonoDomain;
}

namespace Shard3D {
	typedef int ScriptLanguage;
#define ScriptLanguage_CSharp 0
#define ScriptLanguage_VBasic 1
	struct ScriptEngineData;
	class DynamicScriptClass;
	namespace ECS {
		class Level;
		class Actor;
	}
	struct HUDElement;
	class HUDContainer;
	class DynamicScriptEngine {
	public:	
		struct _a {
			void beginEvent(ECS::Actor __a);
			void endEvent();
			void tickEvent(float __dt);

			void spawnEvent(ECS::Actor actor);
			void killEvent(ECS::Actor actor);
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
		static MonoImage* getCoreAssemblyImage(ScriptLanguage lang);
		static MonoDomain* getDomain();
		static hashMap<std::string, sPtr<DynamicScriptClass>> getActorClasses(ScriptLanguage lang);
		static void runtimeStart(ECS::Level* level);
		static void runtimeStop();
		static void setHUDContext(HUDContainer* container);
		static inline bool doesClassExist(const std::string& fullClassName, int lang);
		static inline bool doesHUDClassExist(const std::string& fullClassName, int lang);
		static ECS::Level* getContext();
		static HUDContainer* getHUDContext();
		static _a actorScript() { return _a(); }
		static _h hudScript() { return _h(); }
	private:
		static void _reloadAssembly(ScriptEngineData* scriptEngine, ScriptLanguage lang);
		static void destroyMono(ScriptEngineData* scriptEngine);

		static MonoObject* instClass(MonoClass* monoClass, ScriptLanguage lang);
		static void loadAssemblyClasses(ScriptEngineData* data);
		friend class DynamicScriptClass;
		friend class DynamicScriptEngineLinker;
		friend struct MonoTypeCombo;
	};

	class DynamicScriptClass {
	public:
		DynamicScriptClass() = default;
		DynamicScriptClass(const std::string& c_ns, const std::string& c_n, ScriptLanguage _lang);

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
			ScriptEvents(sPtr<DynamicScriptClass> ptr, MonoObject* i);
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

			sPtr<DynamicScriptClass> s_c;
			MonoObject* _i{};
		};
		ScriptEvents scriptEvents;
	public:	
		DynamicScriptInstance(sPtr<DynamicScriptClass> s_class, ScriptLanguage lang, ECS::Actor actor);
		ScriptEvents invokeEvent();
	private:
		sPtr<DynamicScriptClass> scriptClass;

		MonoMethod* constructor{};
		MonoObject* instance{};		
	};
	class HUDScriptInstance {
		struct ScriptEvents {
			ScriptEvents() = default;
			ScriptEvents(sPtr<DynamicScriptClass> ptr, MonoObject* i);
			void hoverEvent(float dt);
			void pressEvent(float dt);
			void clickEvent();

			MonoMethod* hoverEventMethod{};
			MonoMethod* pressEventMethod{};
			MonoMethod* clickEventMethod{};

			sPtr<DynamicScriptClass> s_c;
			MonoObject* _i{};
		};
		ScriptEvents scriptEvents;
	public:	
		HUDScriptInstance(sPtr<DynamicScriptClass> s_class, int lang, HUDElement* element);
		ScriptEvents invokeEvent();
	private:
		sPtr<DynamicScriptClass> scriptClass;

		MonoMethod* constructor{};
		MonoObject* instance{};
	};
}