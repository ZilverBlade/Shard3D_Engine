workspace "Shard3D"
   architecture "x86_64"
   preferredtoolarchitecture "x86_64"
   configurations { "Debug", "Release" }

local outputSubfolder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
local VULKAN_SDK = os.getenv("VULKAN_SDK")

local globalInclude = {
	  VULKAN_SDK.."/Include",
	  "engine/extern/glfw-3.3.7/include", 
	  "engine/extern/spdlog", 
	  "engine/extern/joltphysics/include",
	  "engine/extern/yaml/include",
	  "engine/extern/entt/1include", 
	  "engine/extern/stb/include", 
	  "engine/extern/miniaudio/1include", 
	  "engine/extern/mono/include", 
	  "engine/extern/assimp/include", 
	  "engine/extern/half"
   }

project "Shard3D"
   location "engine/Shard3D"
   debugdir "./"
   local bin = "engine/Shard3D/bin/"..outputSubfolder

   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   targetdir (bin)

   flags { "MultiProcessorCompile" }
   disablewarnings { "26812;4244;4996;4005;" }
   
   files {
      "engine/Shard3D/**.cpp",
      "engine/Shard3D/**.h",
      
	  "engine/Shard3D/core/asset/**.*",
	  "engine/Shard3D/core/audio/**.*",
      "engine/Shard3D/core/ecs/**.*",
	  "engine/Shard3D/core/misc/**.*",
      "engine/Shard3D/core/rendering/**.*",
	  "engine/Shard3D/core/ui/**.*",
      "engine/Shard3D/core/vulkan_api/**.*",
      
	  "engine/Shard3D/events/**.h",
	  "engine/Shard3D/plugins/**.h",
	  
	  
      "engine/Shard3D/scripting/**.cpp",
      "engine/Shard3D/scripting/**.h",
	  
	  "engine/Shard3D/systems/systems.h",
	  "engine/Shard3D/systems/buffers/**.*",
	  "engine/Shard3D/systems/computational/**.*",
	  "engine/Shard3D/systems/rendering/**.*",
	  "engine/Shard3D/systems/rendering/editor/**.*",
	 
      "engine/Shard3D/utils/**.h",
      "engine/Shard3D/utils/**.cpp"
   }

   includedirs {
	  globalInclude
   }
   libdirs {
      VULKAN_SDK.."/Lib",
	  "engine/extern/glfw-3.3.7/lib-vc2019", 
	  "engine/extern/imgui/lib", 
	  "engine/extern/imgui-node-editor/lib", 
	  "engine/extern/yaml/lib", 
	  "engine/extern/mono/lib", 
	  "engine/extern/joltphysics/lib", 
	  "engine/extern/assimp/lib"
   }
   links {
      "vulkan-1",
	  "glfw3"
   }

   filter { "system:windows" }
      links {
	   "Ws2_32",
	   "Winmm",
	   "Bcrypt",
	   "Version",
	   "Comdlg32", 
	   "shlwapi"
      }
	
   filter "configurations:Debug"
      defines { "_DEBUG", "_LIB" }
      runtime "Debug"
      symbols "On"
	  links {
	   "jolt_d",
	   "shadercd",
	   "shaderc_combinedd",
	   "shaderc_sharedd",
	   "shaderc_utild",
	   "libmono-static-sgen_d",
	   "assimp-vc143-mtd",
	   "yaml-cpp_d"
	  }

   filter "configurations:Release"
	  defines { "NDEBUG" }
      runtime "Release"
      optimize "Speed"
	  links {
	   "jolt",
	   "shaderc",
	   "shaderc_combined",
	   "shaderc_shared",
	   "shaderc_util",
	   "libmono-static-sgen",
	   "assimp-vc143-mt",
	   "yaml-cpp"
	  }
	  
	  
project "Shard3DEditor"
   location "editor"
   debugdir "./"
   
   local bin = "export/bin/"..outputSubfolder
  
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir (bin)

   flags { "MultiProcessorCompile" }
   disablewarnings { "26812;4244;4996;4005;" }
   
   files {
	  "editor/application.h",
	  "editor/application.cpp",
	  "editor/main.cpp",
	  "editor/camera/**.*",
	  "editor/imgui/**.*",
	  "editor/panels/**.*",
	  "editor/camera/**.*",
   }

   includedirs {
	  globalInclude,
	  "engine",
      "editor/extern/imgui",
	  "editor/extern/imguizmo"
   }
   libdirs {
     "editor/extern/imgui/lib",
	 "editor/extern/imguizmo/lib",
	 "engine/Shard3D/bin/"..outputSubfolder
   }
   links {
	  "Shard3D", 
	  "imguizmo"
   }

   filter { "system:windows" }
      links {  
	  "kernel32", 
	  "user32", 
	  "gdi32", 
	  "winspool", 
	  "comdlg32", 
	  "advapi32", 
	  "shell32", 
	  "ole32", 
	  "oleaut32", 
	  "uuid", 
	  "odbc32", 
	  "odbccp32"
      }
	
   filter "configurations:Debug"
	  defines { "_DEBUG" }
      runtime "Debug"
      symbols "On"
	  links {
		"ImGui_d"
	  }

   filter "configurations:Release"
	  defines { "NDEBUG" }
      runtime "Release"
      optimize "Speed"
	  links {
	   "ImGui"
	  }
	  
project "Shard3DRuntime"
   location "runtime"
   debugdir "./"
   
   local bin = "export/bin/"..outputSubfolder
  
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir (bin)

   flags { "MultiProcessorCompile" }
   disablewarnings { "26812;4244;4996;4005;" }
   
   files {
	  "runtime/application.h",
	  "runtime/application.cpp",
	  "runtime/main.cpp",
   }

   includedirs {
	  globalInclude,
	  "engine"
   }
   libdirs {
	 "engine/Shard3D/bin/"..outputSubfolder
   }
   links {
	  "Shard3D"
   }

   filter { "system:windows" }
      links {  
	  "kernel32", 
	  "user32", 
	  "gdi32", 
	  "winspool", 
	  "comdlg32", 
	  "advapi32", 
	  "shell32", 
	  "ole32", 
	  "oleaut32", 
	  "uuid", 
	  "odbc32", 
	  "odbccp32"
      }
	
   filter "configurations:Debug"
	  defines { "_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
	  defines { "NDEBUG" }
      runtime "Release"
      optimize "Speed"