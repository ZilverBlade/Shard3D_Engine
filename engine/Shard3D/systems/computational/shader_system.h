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
		TesselationCtrl = 5,
		TesselationEval = 6,
		Geometry = 4,
		Pixel = Fragment,
		Compute = 3	// not supported, however is planned to make it
		// RT shaders not supported in this engine, nor will they in the near future
	};
	class ShaderSystem {
	public:
		static void init();
		static void compileFromFile(const std::string& source, const std::string& destination, ShaderType type);
		static void compileDirect(const char*, const std::string& destination, ShaderType type);
		static void compileOnTheFly(const std::string& source, ShaderType type);
		static void destroy();
	private:
		static inline shaderc::Compiler* compiler{};
		static inline shaderc::CompileOptions* options{};
	};
}