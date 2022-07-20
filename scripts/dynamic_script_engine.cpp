#include "../s3dtpch.h"

#include "dynamic_script_engine.hpp"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "../utils/definitions.hpp"
#include "dynamic_script_engine_linker.hpp"
#include <filesystem>
#include <fstream>

namespace Shard3D {
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

		static void printAssemblyTypes(MonoAssembly* assembly, int lang)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			if (numTypes > 1) if (lang == 0)	 SHARD3D_LOG("Loaded C# Assembly modules: ");
			else SHARD3D_LOG("Loaded VB Assembly modules: ");
			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				std::printf("\t%s.%s\n", nameSpace, name);
			}
		}
	}

	struct ScriptEngineData {
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;
		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;
	};

	static ScriptEngineData* scriptEngineData;
	static ScriptEngineData* vbScriptEngineData;

	void DynamicScriptEngine::init() {
		scriptEngineData = new ScriptEngineData();
		vbScriptEngineData = new ScriptEngineData();

		mono_set_assemblies_path(ENGINE_MONO_SCRIPT_ASSEMBLY_PATH);
		MonoDomain* rootDomain = mono_jit_init("Shard3D_JIT_Runtime");
		if (rootDomain == nullptr) {
			SHARD3D_FATAL("Failed to initialize Shard3D_JIT_Runtime!!!");
		}

		scriptEngineData->rootDomain = rootDomain;
		vbScriptEngineData->rootDomain = rootDomain;


		reloadAssembly(Language::CSharp);
		reloadAssembly(Language::VisualBasic);

		DynamicScriptEngineLinker::registerLinker();

		initMono("CS", scriptEngineData);
		initMono("VB", vbScriptEngineData);
	}

	void DynamicScriptEngine::destroy() {
		destroyMono(scriptEngineData);
		destroyMono(vbScriptEngineData);
		//mono_jit_cleanup(scriptEngineData->rootDomain); // script
		delete scriptEngineData;
		delete vbScriptEngineData;
	}

	void DynamicScriptEngine::reloadAssembly(Language lang) {
		if (lang == Language::CSharp) {
			_reloadAssembly(scriptEngineData, lang);
		}
		else if (lang == Language::VisualBasic) {
			_reloadAssembly(vbScriptEngineData, lang);
		}
	}
	
	inline void DynamicScriptEngine::_reloadAssembly(ScriptEngineData* scriptEngine, Language lang) {
		scriptEngine->appDomain = mono_domain_create_appdomain((char*)((lang == Language::CSharp) ? "Shard3D_CS_Runtime" : "Shard3D_VB_Runtime"), nullptr);
		mono_domain_set(scriptEngine->appDomain, true);
		scriptEngine->coreAssembly = MonoUtils::loadAssembly((lang == Language::CSharp) ? ENGINE_CS_SCRIPT_RUNTIME_DLL : ENGINE_VB_SCRIPT_RUNTIME_DLL);
		MonoUtils::printAssemblyTypes(scriptEngine->coreAssembly, (int)lang);
	}

	void DynamicScriptEngine::initMono(const char* lang, ScriptEngineData* scriptEngine) {
		scriptEngine->coreAssemblyImage = mono_assembly_get_image(scriptEngine->coreAssembly);
		MonoClass* monoClass = mono_class_from_name(scriptEngine->coreAssemblyImage,
			std::string("Shard3D.Scripts").c_str(), // default namespace 
			"Main");
		MonoObject* objectInst = mono_object_new(scriptEngine->appDomain, monoClass);

		mono_runtime_object_init(objectInst);

		float val = 0.23;
		void* params[1]{ &val };
		MonoMethod* method = mono_class_get_method_from_name(monoClass, "FuncCall", 1);
		mono_runtime_invoke(method, objectInst, params, nullptr);

	}
	void DynamicScriptEngine::destroyMono(ScriptEngineData* scriptEngine) {
		//unload is hard >w<
		//mono_jit_cleanup(scriptEngine->rootDomain);
		scriptEngine->rootDomain = nullptr;
		//mono_domain_unload(scriptEngine->appDomain);
		scriptEngine->appDomain = nullptr;
	}

}