#pragma once
#include "utils/versioning.h"

namespace Shard3D {

//=====================================================================================================================
//=====================================================================================================================

// Definitions are engine only, requires recompiling static library to take effect

const static Version ENGINE_VERSION = { VersionState::Alpha, 1, 2, 9, 1 };	// Shard3D 

#define ENGINE_SETTINGS_PATH						"assets/configdata/engine_settings.ini"
#define EDITOR_SETTINGS_PATH						"assets/configdata/editor_settings.ini"
#define GAME_SETTINGS_PATH							"assets/configdata/game_settings.ini"	

#define ENGINE_ASSETS_PATH							"assets"
#define ENGINE_ASSET_SUFFIX							".s3dasset"
#define ENGINE_SHADER_FILES_PATH					"assets/shaderdata/"				//Shader files (SPIR-V)
#define ENGINE_CORE_ASSETS_PATH						"assets/_engine/"	
#define ENGINE_MONO_SCRIPT_ASSEMBLY_PATH			"assets/scriptdata"
#define ENGINE_SCRIPTENGINECORELIB_DLL				"assets/scriptdata/script/s3dcorelib.dll"
#define ENGINE_FONT_PATH							"assets/_engine/fnt/"		//Fonts
#define ENGINE_DEFAULT_ENGINE_FONT					"assets/_engine/fnt/fs-tahoma-8px.ttf"		//Default engine font
#define ENGINE_FONT_SIZE							16		//size in px
#define ENGINE_SHARD3D_LEVELFILE_OPTIONS			"Shard3D Level (*.s3dlevel)\0*.s3dlevel\0All files (*.*)\0*.*\0"
#define ENGINE_SHARD3D_ASSETFILE_OPTIONS			"Shard3D Asset (*.s3dasset)\0*.s3dasset\0All files (*.*)\0*.*\0"
#define ENGINE_SHARD3D_HUDFILE_OPTIONS				"Shard3D HUD (*.s3dhud)\0*.s3dhud\0All files (*.*)\0*.*\0"
#define ENGINE_MAX_POINTLIGHTS						256		// dont change because these dont match shader code
#define ENGINE_MAX_SPOTLIGHTS						256 
#define ENGINE_MAX_DIRECTIONAL_LIGHTS				16

//=====================================================================================================================
//=====================================================================================================================

// tools
#define ENSET_BETA_DEBUG_TOOLS		

#ifdef NDEBUG
#define SHARD3D_NO_ASSERT	// Disable assertions for slight performance boost
#endif
}