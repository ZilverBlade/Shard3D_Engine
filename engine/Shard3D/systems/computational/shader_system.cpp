#include "../../s3dpch.h"
#include "shader_system.h"
#include <shaderc/shaderc.hpp>
#include <fstream>

namespace Shard3D {
	class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface {
		shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override {
			//BS
			std::string msg = std::string(requesting_source);
			msg += std::to_string(type);
			msg += static_cast<char>(include_depth);

			const std::string name = std::string(requested_source);
			const std::string contents = IOUtils::readText(name, true);

			auto container = new std::array<std::string, 2>;
			(*container)[0] = name;
			(*container)[1] = contents;

			auto data = new shaderc_include_result;

			data->user_data = container;

			data->source_name = (*container)[0].data();
			data->source_name_length = (*container)[0].size();

			data->content = (*container)[1].data();
			data->content_length = (*container)[1].size();

			return data;
		};

		void ReleaseInclude(shaderc_include_result* data) override {
			delete static_cast<std::array<std::string, 2>*>(data->user_data);
			delete data;
		};
	};


	void ShaderSystem::init() {
		compiler = new shaderc::Compiler();
		options = new shaderc::CompileOptions();
		options->AddMacroDefinition("SHARD3D_VERSION", ENGINE_VERSION.toString());
		options->SetOptimizationLevel(shaderc_optimization_level_performance); // always max performance
		options->SetIncluder(std::make_unique<ShaderIncluder>());
	}
	void ShaderSystem::compileFromFile(const std::string& source, const std::string& destination, ShaderType type) {
		SHARD3D_ASSERT(options && compiler && "Shader System has not been initialized!");		
		shaderc::PreprocessedSourceCompilationResult result =
			compiler->PreprocessGlsl(source, (shaderc_shader_kind)type, source.c_str(), *options);

		std::ifstream in(source);
		std::stringstream strStream;
		strStream << in.rdbuf();
		
		shaderc::SpvCompilationResult module =
			compiler->CompileGlslToSpv(strStream.str(), (shaderc_shader_kind)type, source.c_str(), *options);

		in.close();

		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			SHARD3D_FATAL(fmt::format("Failed compiling shader '{}'! {}", source, result.GetErrorMessage()));
		}
	}
	void ShaderSystem::compileDirect(const char* data, const std::string& destination, ShaderType type) {
		SHARD3D_ASSERT(options && compiler && "Shader System has not been initialized!");

		shaderc::SpvCompilationResult result =
			compiler->CompileGlslToSpv(data, (shaderc_shader_kind)type, destination.c_str(), *options);

		std::ofstream out(destination, std::ios::binary);

		out << reinterpret_cast<const char*>(result.cbegin());

		out.flush();
		out.close();
	}
	std::vector<char> ShaderSystem::compileOnTheFly(const std::string& data, ShaderType type) {
		SHARD3D_ASSERT(options && compiler && "Shader System has not been initialized!");
		
		SHARD3D_INFO("Compiling Shader '{0}'", data);
		std::string shaderCode = IOUtils::readText(data, true);
		shaderc::PreprocessedSourceCompilationResult pre_result =
			compiler->PreprocessGlsl(shaderCode, (shaderc_shader_kind)type, data.c_str(), *options);
		auto rtype = (shaderc_shader_kind)type;
		shaderc::SpvCompilationResult result = compiler->CompileGlslToSpv(pre_result.begin(), (shaderc_shader_kind)type, data.c_str(), *options);
	
		if (pre_result.GetNumErrors() > 0) {
			SHARD3D_ERROR(pre_result.GetErrorMessage());
		}

		if (result.GetNumErrors() > 0) {
			SHARD3D_ERROR(result.GetErrorMessage());
		}

		std::vector<uint32_t> sresult = std::vector<uint32_t>(result.cbegin(), result.cend());
		return *reinterpret_cast<std::vector<char>*>(&sresult);
	}
	std::vector<char> ShaderSystem::compileOnTheFlyDirect(const std::string& shaderCode, const char* sourceFile, ShaderType type) {
		SHARD3D_ASSERT(options && compiler && "Shader System has not been initialized!");
		
		SHARD3D_INFO("Compiling Shader '{0}'", sourceFile);
		shaderc::PreprocessedSourceCompilationResult pre_result =
			compiler->PreprocessGlsl(shaderCode, (shaderc_shader_kind)type, sourceFile, *options);
		std::string xresult = std::string(pre_result.cbegin(), pre_result.cend());
		std::string fixed = xresult.substr(0, xresult.find_last_of("}") + 1);
		shaderc::SpvCompilationResult result = compiler->CompileGlslToSpv(fixed.data(), fixed.size(), (shaderc_shader_kind)type, sourceFile, *options);

		if (pre_result.GetNumErrors() > 0) {
			SHARD3D_ERROR(pre_result.GetErrorMessage());
		}
		
		if (result.GetNumErrors() > 0) {
			SHARD3D_ERROR(result.GetErrorMessage());
		}

		std::vector<uint32_t> sresult = std::vector<uint32_t>(result.cbegin(), result.cend());
		return *reinterpret_cast<std::vector<char>*>(&sresult);
	}
	void ShaderSystem::destroy() {
		delete compiler;
		delete options;
	}
}