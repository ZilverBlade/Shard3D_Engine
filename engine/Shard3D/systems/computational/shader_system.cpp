#include "../../s3dpch.h"
#include "shader_system.h"
#include <shaderc/shaderc.hpp>
#include <fstream>

namespace Shard3D {
	void ShaderSystem::init() {
		compiler = new shaderc::Compiler();
		options = new shaderc::CompileOptions();
		options->AddMacroDefinition("SHARD3D_VERSION", ENGINE_VERSION.toString());
		options->AddMacroDefinition("SHARD3D_SHADER_TEMPLATE");
		options->SetOptimizationLevel(shaderc_optimization_level_performance); // always max performance
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
	const char* ShaderSystem::compileOnTheFly(const std::string& data, ShaderType type) {
		shaderc::SpvCompilationResult result =
			compiler->CompileGlslToSpv(data, (shaderc_shader_kind)type, "assets/_engine/0.spv", *options);
		return reinterpret_cast<const char*>(result.cbegin());
	}
	void ShaderSystem::destroy() {
		delete compiler;
		delete options;
	}
}