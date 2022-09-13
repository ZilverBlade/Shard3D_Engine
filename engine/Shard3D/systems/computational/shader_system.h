#pragma once
#include <string>
namespace shaderc {
	class Compiler;
	class CompileOptions;
}
namespace Shard3D {
	enum class ShaderType {
		Vertex = 0,
		Fragment = 1,
		TesselationCtrl = 4,
		TesselationEval = 5,
		Geometry = 3,
		Pixel = Fragment,
		Compute = 2	// not supported, however is planned to make it
		// RT shaders not supported in this engine, nor will they in the near future
	};
	class ShaderSystem {
	public:
		static void init();
		static void compileFromFile(const std::string& source, const std::string& destination, ShaderType type);
		static void compileDirect(const char*, const std::string& destination, ShaderType type);
		static std::vector<char> compileOnTheFly(const std::string& source, ShaderType type);
		static std::vector<char> compileOnTheFlyDirect(const std::string& shaderCode, const char* sourceFile, ShaderType type);
		static void destroy();
	private:
		static inline shaderc::Compiler* compiler{};
		static inline shaderc::CompileOptions* options{};
	};
}