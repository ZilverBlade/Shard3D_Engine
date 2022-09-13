#include "../s3dpch.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/image.h>
#include <mono/metadata/class.h>

#include <filesystem>
#include <fstream>

#include "script_engine.h"
#include "script_engine_linker.h"

#include "../core.h"
#include "../core/misc/UUID.h"
#include "../core/ecs/actor.h"
#include "../core/ecs/components.h"
#include "../core/ui/hud.h"
#include "../core/misc/graphics_settings.h"
namespace Shard3D {

	struct GlobalScriptEngineData {
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;
		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr; 

		ScriptClass actorClass;
		ScriptClass hudClass;

		ECS::Level* levelContext{};
		HUDContainer* hudContext{};
	};

	struct ScriptEngineData {
		MonoAssembly* appAssembly = nullptr;
		MonoImage* appAssemblyImage = nullptr;

		ScriptLanguage lang;

		hashMap<std::string, sPtr<ScriptClass>> actorClasses;
		hashMap<UUID, sPtr<ScriptInstance>> actorInstances;

		hashMap<std::string, sPtr<ScriptClass>> hudClasses;
		hashMap<UUID, sPtr<HUDScriptInstance>> hudInstances;
	};

	static ScriptEngineData* scriptEngineData;
	static ScriptEngineData* vbScriptEngineData;
	static GlobalScriptEngineData* globalData;

	namespace MonoUtils {
		static char* readBytes(const std::string& filepath, uint32_t* outSize) {
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* loadAssembly(const std::string& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = readBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		static void printAssemblyTypes(ScriptEngineData* data, MonoAssembly* assembly, int lang)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			if (numTypes > 1) if (lang == 0)	 SHARD3D_LOG("Loaded C# Assembly modules: ");
			else SHARD3D_LOG("Loaded VB Assembly modules: ");
			for (int32_t i = 0; i < numTypes; i++) {
				if (i == 0) continue;
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
				if (!strUtils::hasStarting(nameSpace, "Shard3D.")) continue; // dont log core stuff
				std::printf("\t%s.%s\n", nameSpace, name);
			}
		}
	}
	void ScriptEngine::loadAssemblyClasses(ScriptEngineData* data) {
		data->actorClasses.clear();
		data->hudClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(data->appAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* actorClass = mono_class_from_name(globalData->coreAssemblyImage, "Shard3D.Core", "Actor");
		
	
		for (int32_t i = 0; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(data->appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(data->appAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullname = fmt::format("{}.{}", nameSpace, name);

			MonoClass* monoClass{};
			monoClass = mono_class_from_name(data->appAssemblyImage, nameSpace, name);
			if (strUtils::hasStarting(nameSpace, "My") || strUtils::hasStarting(name, "MyWebServices") || strUtils::hasStarting(name, "Thread")) continue; // dont log core stuff
			
				//wtf is this bullshit not recognising

			bool isActor = mono_class_is_subclass_of(monoClass, actorClass, false);

			sPtr<ScriptClass> ptr = make_sPtr<ScriptClass>(nameSpace, name, (int)data->lang);
			if (isActor) {
				data->actorClasses[fullname] = ptr;
			}
			SHARD3D_LOG("is {0} actor? {1} (lang {2})", fullname, isActor, data->lang == ScriptLanguage_CSharp? "C#" : "VB");
		}

		MonoClass* hudClass = mono_class_from_name(globalData->coreAssemblyImage, "Shard3D.Core", "HUD");
		for (int32_t i = 0; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(data->appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(data->appAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullname = fmt::format("{}.{}", nameSpace, name);

			MonoClass* monoClass{};
			monoClass = mono_class_from_name(data->appAssemblyImage, nameSpace, name);
			if (monoClass == actorClass) continue;
			if (strUtils::hasStarting(nameSpace, "My") || strUtils::hasStarting(name, "MyWebServices") || strUtils::hasStarting(name, "Thread")) continue; // dont log core stuff
			bool isHUD = mono_class_is_subclass_of(monoClass, hudClass, false);
			sPtr<ScriptClass> ptr = make_sPtr<ScriptClass>(nameSpace, name, (int)data->lang);
			if (isHUD) {
				data->hudClasses[fullname] = ptr;
			}
		}
	}

	MonoImage* ScriptEngine::getCoreAssemblyImage() {
		return globalData->coreAssemblyImage;
	}

	MonoDomain* ScriptEngine::getDomain() {
		return globalData->rootDomain;
	}

	MonoObject* ScriptEngine::instClass(MonoClass* monoClass) {
		MonoObject* instance = mono_object_new(globalData->appDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::init() {
		scriptEngineData = new ScriptEngineData();
		scriptEngineData->lang = ScriptLanguage_CSharp;
		vbScriptEngineData = new ScriptEngineData();
		vbScriptEngineData->lang = ScriptLanguage_VBasic;

		globalData = new GlobalScriptEngineData();

		mono_set_assemblies_path(ENGINE_MONO_SCRIPT_ASSEMBLY_PATH);
		MonoDomain* rootDomain = mono_jit_init("Shard3D_JIT_Runtime");
		if (rootDomain == nullptr) {
			SHARD3D_FATAL("Failed to initialize Shard3D_JIT_Runtime!!!");
		}

		globalData->rootDomain = rootDomain;

		globalData->appDomain = mono_domain_create_appdomain((char*)("Shard3D_Runtime"), nullptr);
		mono_domain_set(globalData->appDomain, true);
		_loadCoreAssembly();
		
		reloadAssembly(ScriptLanguage_CSharp);
		reloadAssembly(ScriptLanguage_VBasic);
		
		globalData->actorClass = ScriptClass("Shard3D.Core", "Actor");
		globalData->hudClass = ScriptClass("Shard3D.Core", "HUD");

		loadAssemblyClasses(scriptEngineData);
		loadAssemblyClasses(vbScriptEngineData);
		ScriptEngineLinker::registerLinker();

	}

	void ScriptEngine::destroy() {
		globalData->rootDomain = nullptr;
		destroyMono(scriptEngineData);
		destroyMono(vbScriptEngineData);
		//mono_jit_cleanup(scriptEngineData->rootDomain); // script
		delete scriptEngineData;
		delete vbScriptEngineData;
		delete globalData;
	}

	void ScriptEngine::reloadAssembly(ScriptLanguage lang) {
		if (lang == 0) {
			_reloadAssembly(scriptEngineData, lang);
		}
		else if (lang == 1) {
			_reloadAssembly(vbScriptEngineData, lang);
		}
	}

	void ScriptEngine::_loadCoreAssembly() {
		globalData->coreAssembly = MonoUtils::loadAssembly(ENGINE_SCRIPTENGINECORELIB_DLL);
		globalData->coreAssemblyImage = mono_assembly_get_image(globalData->coreAssembly);

		MonoImage* image = mono_assembly_get_image(globalData->coreAssembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		SHARD3D_LOG("Loaded Core Assembly modules: ");
		for (int32_t i = 0; i < numTypes; i++) {
			if (i == 0) continue;
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			std::printf("\t%s.%s\n", nameSpace, name);
		}
	}

	inline void ScriptEngine::_reloadAssembly(ScriptEngineData* scriptEngine, ScriptLanguage lang) {
		std::string path = std::string("assets/scriptdata/script/") + GraphicsSettings::getStaticApplicationInfo().gameName + std::string((lang == ScriptLanguage_CSharp) ? "-cs.dll" : "-vb.dll");
		scriptEngine->appAssembly = MonoUtils::loadAssembly(path);
		scriptEngine->appAssemblyImage = mono_assembly_get_image(scriptEngine->appAssembly);
		MonoUtils::printAssemblyTypes(scriptEngine, scriptEngine->appAssembly, (int)lang);
	}


	hashMap<std::string, sPtr<ScriptClass>> ScriptEngine::getActorClasses(ScriptLanguage lang) {
		return !lang ? scriptEngineData->actorClasses : vbScriptEngineData->actorClasses;
	}

	void ScriptEngine::runtimeStart(ECS::Level* level) {
		globalData->levelContext = level;
	}

	void ScriptEngine::runtimeStop() {
		globalData->levelContext = nullptr;

		scriptEngineData->actorInstances.clear();
		vbScriptEngineData->actorInstances.clear();
		scriptEngineData->hudInstances.clear();
		vbScriptEngineData->hudInstances.clear();
	}

	void ScriptEngine::setHUDContext(HUDContainer* container) {
		globalData->hudContext = container;
	}

	bool ScriptEngine::doesClassExist(const std::string& fullClassName, int lang) {
		return !lang ? scriptEngineData->actorClasses.find(fullClassName) != scriptEngineData->actorClasses.end() :
			vbScriptEngineData->actorClasses.find(fullClassName) != vbScriptEngineData->actorClasses.end();
	}

	bool ScriptEngine::doesHUDClassExist(const std::string& fullClassName, int lang) {
		return !lang ? scriptEngineData->hudClasses.find(fullClassName) != scriptEngineData->hudClasses.end() :
			vbScriptEngineData->hudClasses.find(fullClassName) != vbScriptEngineData->hudClasses.end();
	}

	ECS::Level* ScriptEngine::getContext() {
		return	globalData->levelContext;
	}

	HUDContainer* ScriptEngine::getHUDContext() {
		return globalData->hudContext;
	}

	void ScriptEngine::destroyMono(ScriptEngineData* scriptEngine) {
		//unload is hard >w<
		//mono_jit_cleanup(scriptEngine->rootDomain);

		//mono_domain_unload(scriptEngine->appDomain);
		globalData->appDomain = nullptr;
	}

	void ScriptEngine::_a::beginEvent(ECS::Actor __a) {
		const auto& scr = __a.getComponent<Components::ScriptComponent>();
		
		if (doesClassExist("Shard3D.Scripts." + scr.name, scr.lang)) {
			auto& data = !scr.lang ? scriptEngineData : vbScriptEngineData;
			sPtr<ScriptInstance> instance = make_sPtr<ScriptInstance>(data->actorClasses["Shard3D.Scripts." + scr.name], scr.lang, __a);
			data->actorInstances[__a.getUUID()] = instance;
			instance->invokeEvent().beginEvent();
		}
	}

	void ScriptEngine::_a::endEvent() {
		{ // C#
			for (auto& actor : scriptEngineData->actorInstances) {
				actor.second->invokeEvent().endEvent();
			}
		}
		{ // Visual Basic
			for (auto& actor : vbScriptEngineData->actorInstances) {
				actor.second->invokeEvent().endEvent();
			}
		}
	}

	void ScriptEngine::_a::tickEvent(float __dt) {
		{ // C#
			for (auto& actor : scriptEngineData->actorInstances) {
				actor.second->invokeEvent().tickEvent(__dt);
			}
		}
		{ // Visual Basic
			for (auto& actor : vbScriptEngineData->actorInstances) {
				actor.second->invokeEvent().tickEvent(__dt);
			}
		}
	}

	void ScriptEngine::_a::spawnEvent(ECS::Actor actor) {
		{ // C#
			const auto guid = actor.getUUID();
			if (scriptEngineData->actorInstances.find(guid) != scriptEngineData->actorInstances.end()) 
				scriptEngineData->actorInstances[guid]->invokeEvent().spawnEvent();
		}
		{ // Visual Basic
			const auto guid = actor.getUUID();
			if (vbScriptEngineData->actorInstances.find(guid) != vbScriptEngineData->actorInstances.end())
				vbScriptEngineData->actorInstances[guid]->invokeEvent().spawnEvent();
		}
	}

	void ScriptEngine::_a::killEvent(ECS::Actor actor) {
		{ // C#
			const auto guid = actor.getUUID();
			if (scriptEngineData->actorInstances.find(guid) != scriptEngineData->actorInstances.end()) {
				scriptEngineData->actorInstances[guid]->invokeEvent().killEvent();
				scriptEngineData->actorInstances.erase(guid);
			}
		}
		{ // Visual Basic
			const auto guid = actor.getUUID();
			if (vbScriptEngineData->actorInstances.find(guid) != vbScriptEngineData->actorInstances.end()) {
				vbScriptEngineData->actorInstances[guid]->invokeEvent().killEvent();
				vbScriptEngineData->actorInstances.erase(guid);
			}
		}
	}

	void ScriptEngine::_h::begin(HUDElement* __h) {
		const std::string& scr = __h->scriptmodule;
		const int& scrl = __h->scriptlang;

		if (doesHUDClassExist("Shard3D.UI." + scr, scrl)) {
			auto& data = !scrl ? scriptEngineData : vbScriptEngineData;
			sPtr<HUDScriptInstance> instance = make_sPtr<HUDScriptInstance>(data->hudClasses["Shard3D.UI." + scr], scrl, __h);
			data->hudInstances[__h->guid] = instance;
		}
	}
	void ScriptEngine::_h::end(HUDElement* __h) {

	}
	void ScriptEngine::_h::hoverEvent(HUDElement* __h, float __dt) {
		{ // C#
			const auto guid = __h->guid;
			if (scriptEngineData->hudInstances.find(guid) != scriptEngineData->hudInstances.end())
				scriptEngineData->hudInstances[guid]->invokeEvent().hoverEvent(__dt);
		}
		{ // Visual Basic
			const auto guid = __h->guid;
			if (vbScriptEngineData->hudInstances.find(guid) != vbScriptEngineData->hudInstances.end())
				vbScriptEngineData->hudInstances[guid]->invokeEvent().hoverEvent(__dt);
		}
	}

	void ScriptEngine::_h::pressEvent(HUDElement* __h, float __dt) {
		{ // C#
			const auto guid = __h->guid;
			if (scriptEngineData->hudInstances.find(guid) != scriptEngineData->hudInstances.end())
				scriptEngineData->hudInstances[guid]->invokeEvent().pressEvent(__dt);
		}
		{ // Visual Basic
			const auto guid = __h->guid;
			if (vbScriptEngineData->hudInstances.find(guid) != vbScriptEngineData->hudInstances.end())
				vbScriptEngineData->hudInstances[guid]->invokeEvent().pressEvent(__dt);
		}
	}

	void ScriptEngine::_h::clickEvent(HUDElement* __h) {
		{ // C#
			const auto guid = __h->guid;
			if (scriptEngineData->hudInstances.find(guid) != scriptEngineData->hudInstances.end())
				scriptEngineData->hudInstances[guid]->invokeEvent().clickEvent();
		}
		{ // Visual Basic
			const auto guid = __h->guid;
			if (vbScriptEngineData->hudInstances.find(guid) != vbScriptEngineData->hudInstances.end())
				vbScriptEngineData->hudInstances[guid]->invokeEvent().clickEvent();
		}
	}
}
namespace Shard3D {
	ScriptClass::ScriptClass(const std::string& c_ns, const std::string& c_n, ScriptLanguage _lang) : classNamespace(c_ns), className(c_n) {
		monoClass = mono_class_from_name(!_lang ? scriptEngineData->appAssemblyImage : vbScriptEngineData->appAssemblyImage, c_ns.c_str(), c_n.c_str());
	}
	ScriptClass::ScriptClass(const std::string& c_ns, const std::string& c_n) : classNamespace(c_ns), className(c_n) {
		monoClass = mono_class_from_name(globalData->coreAssemblyImage, c_ns.c_str(), c_n.c_str());
	}
	MonoObject* ScriptClass::inst() {
		return ScriptEngine::instClass(monoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int parameterCount) {
		return mono_class_get_method_from_name(monoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params) {
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

	ScriptInstance::ScriptInstance(sPtr<ScriptClass> s_class, ScriptLanguage lang, ECS::Actor actor) : scriptClass(s_class) {
		instance = s_class->inst();

		constructor = globalData->actorClass.getMethod(".ctor", 1);
		scriptEvents = ScriptEvents(s_class, instance);
		{
			uint64_t guid = actor.getUUID();
			void* param = &guid;
			scriptClass->invokeMethod(instance, constructor, &param);
		}
	}
	ScriptInstance::ScriptEvents ScriptInstance::invokeEvent() { return scriptEvents; }
	ScriptInstance::ScriptEvents::ScriptEvents(sPtr<ScriptClass> ptr, MonoObject* i) : s_c(ptr), _i(i) {
		beginEventMethod = s_c->getMethod("BeginEvent", 0);
		endEventMethod = s_c->getMethod("EndEvent", 0);
		tickEventMethod = s_c->getMethod("TickEvent", 1);
		spawnEventMethod = s_c->getMethod("SpawnEvent", 0);
		killEventMethod = s_c->getMethod("KillEvent", 0);
	}
	void ScriptInstance::ScriptEvents::beginEvent() { s_c->invokeMethod(_i, beginEventMethod); }
	void ScriptInstance::ScriptEvents::endEvent() { s_c->invokeMethod(_i, endEventMethod); }
	void ScriptInstance::ScriptEvents::tickEvent(float dt) { void* param = &dt; s_c->invokeMethod(_i, tickEventMethod, &param); }
	void ScriptInstance::ScriptEvents::spawnEvent() { s_c->invokeMethod(_i, spawnEventMethod); }
	void ScriptInstance::ScriptEvents::killEvent() { s_c->invokeMethod(_i, killEventMethod); }

	HUDScriptInstance::HUDScriptInstance(sPtr<ScriptClass> s_class, int lang, HUDElement* element) : scriptClass(s_class) {
		instance = s_class->inst();

		constructor = globalData->hudClass.getMethod(".ctor", 1);
		scriptEvents = ScriptEvents(s_class, instance);
		{
			uint64_t guid = element->guid;
			void* param = &guid;
			scriptClass->invokeMethod(instance, constructor, &param);
		}
	}
	HUDScriptInstance::ScriptEvents HUDScriptInstance::invokeEvent() { return scriptEvents; }
	HUDScriptInstance::ScriptEvents::ScriptEvents(sPtr<ScriptClass> ptr, MonoObject* i) : s_c(ptr), _i(i) {
		hoverEventMethod = s_c->getMethod("HoverEvent", 1);
		pressEventMethod = s_c->getMethod("PressEvent", 1);
		clickEventMethod = s_c->getMethod("ClickEvent", 0);
	}
	void HUDScriptInstance::ScriptEvents::hoverEvent(float dt) { void* param = &dt; s_c->invokeMethod(_i, hoverEventMethod, &param); }
	void HUDScriptInstance::ScriptEvents::pressEvent(float dt) { void* param = &dt; s_c->invokeMethod(_i, pressEventMethod, &param); }
	void HUDScriptInstance::ScriptEvents::clickEvent() { s_c->invokeMethod(_i, clickEventMethod); }
}