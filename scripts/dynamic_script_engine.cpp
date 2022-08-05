#include "../s3dtpch.h"

#include "dynamic_script_engine.hpp"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/image.h>
#include <mono/metadata/class.h>
#include "../utils/definitions.hpp"
#include "dynamic_script_engine_linker.hpp"
#include <filesystem>
#include <fstream>
#include "../GUID.hpp"
#include "../wb3d/actor.hpp"
namespace Shard3D {

	struct GlobalScriptEngineData {
		MonoDomain* rootDomain = nullptr;
		wb3d::Level* levelContext{};

	};

	struct ScriptEngineData {
		MonoDomain* appDomain = nullptr;
		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;

		ScriptLanguage lang;

		DynamicScriptClass actorClass;
		std::unordered_map<std::string, std::shared_ptr<DynamicScriptClass>> actorClasses;
		std::unordered_map<GUID, std::shared_ptr<DynamicScriptInstance>> actorInstances;
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
	void DynamicScriptEngine::loadAssemblyClasses(ScriptEngineData* data) {
		data->actorClasses.clear();

		MonoImage* image = data->coreAssemblyImage;
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* actorClass = mono_class_from_name(image, "Shard3D.Core", "Actor");


		for (int32_t i = 0; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			std::string fullname = fmt::format("{}.{}", nameSpace, name);

			MonoClass* monoClass{};
			monoClass = mono_class_from_name(image, nameSpace, name);
			if (monoClass == actorClass) continue;
			if (strUtils::hasStarting(nameSpace, "My") || strUtils::hasStarting(name, "MyWebServices") || strUtils::hasStarting(name, "Thread")) continue; // dont log core stuff
			bool isActor = mono_class_is_subclass_of(monoClass, actorClass, false);
			std::shared_ptr<DynamicScriptClass> ptr = std::make_shared<DynamicScriptClass>(nameSpace, name, (int)data->lang);
			if (isActor) {
				data->actorClasses[fullname] = ptr;
			}
			//SHARD3D_LOG("is {0} part? {1} ({2})", fullname, isActor, data->lang == ScriptLanguage::CSharp ? "C#" : "VB");
		}
	}

	MonoImage* DynamicScriptEngine::getCoreAssemblyImage(ScriptLanguage lang) {
		return !lang ? scriptEngineData->coreAssemblyImage : vbScriptEngineData->coreAssemblyImage;
	}

	MonoObject* DynamicScriptEngine::instClass(MonoClass* monoClass, ScriptLanguage lang) {
		MonoObject* instance = mono_object_new(!lang ? scriptEngineData->appDomain : vbScriptEngineData->appDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void DynamicScriptEngine::init() {
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

		scriptEngineData->appDomain = mono_domain_create_appdomain((char*)("Shard3D_CS_Runtime"), nullptr);
		vbScriptEngineData->appDomain = mono_domain_create_appdomain((char*)("Shard3D_VB_Runtime"), nullptr);

		reloadAssembly(ScriptLanguage_CSharp);
		reloadAssembly(ScriptLanguage_VBasic);

		
		scriptEngineData->actorClass = DynamicScriptClass("Shard3D.Core", "Actor", 0);
		vbScriptEngineData->actorClass = DynamicScriptClass("Shard3D.Core", "Actor", 1);

		loadAssemblyClasses(scriptEngineData);
		loadAssemblyClasses(vbScriptEngineData);
		DynamicScriptEngineLinker::registerLinker();

	}

	void DynamicScriptEngine::destroy() {
		globalData->rootDomain = nullptr;
		destroyMono(scriptEngineData);
		destroyMono(vbScriptEngineData);
		//mono_jit_cleanup(scriptEngineData->rootDomain); // script
		delete scriptEngineData;
		delete vbScriptEngineData;
	}

	void DynamicScriptEngine::reloadAssembly(ScriptLanguage lang) {
		if (lang == 0) {
			_reloadAssembly(scriptEngineData, lang);
		}
		else if (lang == 1) {
			_reloadAssembly(vbScriptEngineData, lang);
		}
	}

	std::unordered_map<std::string, std::shared_ptr<DynamicScriptClass>> DynamicScriptEngine::getActorClasses(ScriptLanguage lang) {
		return !lang ? scriptEngineData->actorClasses : vbScriptEngineData->actorClasses;
	}

	void DynamicScriptEngine::runtimeStart(wb3d::Level* level) {
		globalData->levelContext = level;
	}

	void DynamicScriptEngine::runtimeStop() {
		globalData->levelContext = nullptr;

		scriptEngineData->actorInstances.clear();
		vbScriptEngineData->actorInstances.clear();
	}

	bool DynamicScriptEngine::doesClassExist(const std::string& fullClassName, ScriptLanguage lang) {
		return !lang ? scriptEngineData->actorClasses.find(fullClassName) != scriptEngineData->actorClasses.end() :
			vbScriptEngineData->actorClasses.find(fullClassName) != vbScriptEngineData->actorClasses.end();
	}

	wb3d::Level* DynamicScriptEngine::getContext() {
		return 	globalData->levelContext;
	}

	inline void DynamicScriptEngine::_reloadAssembly(ScriptEngineData* scriptEngine, ScriptLanguage lang) {
		mono_domain_set(scriptEngine->appDomain, true);
		scriptEngine->coreAssembly = MonoUtils::loadAssembly((lang == ScriptLanguage_CSharp) ? ENGINE_CS_SCRIPT_RUNTIME_DLL : ENGINE_VB_SCRIPT_RUNTIME_DLL);
		scriptEngine->coreAssemblyImage = mono_assembly_get_image(scriptEngine->coreAssembly);
		MonoUtils::printAssemblyTypes(scriptEngine, scriptEngine->coreAssembly, (int)lang);
	}

	void DynamicScriptEngine::destroyMono(ScriptEngineData* scriptEngine) {
		//unload is hard >w<
		//mono_jit_cleanup(scriptEngine->rootDomain);

		//mono_domain_unload(scriptEngine->appDomain);
		scriptEngine->appDomain = nullptr;
	}

	void DynamicScriptEngine::_e::beginEvent(wb3d::Actor _a) {
		const auto& scr = _a.getComponent<Components::ScriptComponent>();

		if (doesClassExist("Shard3D.Scripts." + scr.name, scr.lang)) {
			auto& data = !scr.lang ? scriptEngineData : vbScriptEngineData;
			std::shared_ptr<DynamicScriptInstance> instance = std::make_shared<DynamicScriptInstance>(data->actorClasses["Shard3D.Scripts." + scr.name], scr.lang, _a);
			data->actorInstances[_a.getGUID()] = instance;
			instance->invokeEvent().beginEvent();
		}
	}

	void DynamicScriptEngine::_e::endEvent() {
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

	void DynamicScriptEngine::_e::tickEvent(float __dt) {
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

	void DynamicScriptEngine::_e::spawnEvent() {
		{ // C#
			for (auto& actor : scriptEngineData->actorInstances) {
				actor.second->invokeEvent().spawnEvent();
			}
		}
		{ // Visual Basic
			for (auto& actor : vbScriptEngineData->actorInstances) {
				actor.second->invokeEvent().spawnEvent();
			}
		}
	}

	void DynamicScriptEngine::_e::killEvent() {
		{ // C#
			for (auto& actor : scriptEngineData->actorInstances) {
				actor.second->invokeEvent().killEvent();
			}
		}
		{ // Visual Basic
			for (auto& actor : vbScriptEngineData->actorInstances) {
				actor.second->invokeEvent().killEvent();
			}
		}
	}

}
namespace Shard3D {
	DynamicScriptClass::DynamicScriptClass(const std::string& c_ns, const std::string& c_n, ScriptLanguage _lang) : classNamespace(c_ns), className(c_n), lang(_lang) {
		monoClass = mono_class_from_name(!lang ? scriptEngineData->coreAssemblyImage : vbScriptEngineData->coreAssemblyImage, c_ns.c_str(), c_n.c_str());
	}

	MonoObject* DynamicScriptClass::inst() {
		return DynamicScriptEngine::instClass(monoClass, lang);
	}

	MonoMethod* DynamicScriptClass::getMethod(const std::string& name, int parameterCount) {
		return mono_class_get_method_from_name(monoClass, name.c_str(), parameterCount);
	}

	MonoObject* DynamicScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params) {
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

	DynamicScriptInstance::DynamicScriptInstance(std::shared_ptr<DynamicScriptClass> s_class, ScriptLanguage lang, wb3d::Actor actor) : scriptClass(s_class) {
		instance = s_class->inst();

		constructor = (!lang ? scriptEngineData : vbScriptEngineData)->actorClass.getMethod(".ctor", 1);
		scriptEvents = ScriptEvents(s_class, instance);
		{
			uint64_t guid = actor.getGUID();
			void* param = &guid;
			scriptClass->invokeMethod(instance, constructor, &param);
		}
	}
	DynamicScriptInstance::ScriptEvents DynamicScriptInstance::invokeEvent() { return scriptEvents; }
	DynamicScriptInstance::ScriptEvents::ScriptEvents(std::shared_ptr<DynamicScriptClass> ptr, MonoObject* i) : s_c(ptr), _i(i) {
		beginEventMethod = s_c->getMethod("BeginEvent", 0);
		endEventMethod = s_c->getMethod("EndEvent", 0);
		tickEventMethod = s_c->getMethod("TickEvent", 1);
		spawnEventMethod = s_c->getMethod("SpawnEvent", 0);
		killEventMethod = s_c->getMethod("KillEvent", 0);
	}
	void DynamicScriptInstance::ScriptEvents::beginEvent() { s_c->invokeMethod(_i, beginEventMethod); }
	void DynamicScriptInstance::ScriptEvents::endEvent() { s_c->invokeMethod(_i, endEventMethod); }
	void DynamicScriptInstance::ScriptEvents::tickEvent(float dt) { void* param = &dt; s_c->invokeMethod(_i, tickEventMethod, &param); }
	void DynamicScriptInstance::ScriptEvents::spawnEvent() { s_c->invokeMethod(_i, spawnEventMethod); }
	void DynamicScriptInstance::ScriptEvents::killEvent() { s_c->invokeMethod(_i, killEventMethod); }
}