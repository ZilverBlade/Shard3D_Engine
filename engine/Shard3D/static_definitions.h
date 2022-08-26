#pragma once
#include "utils/versioning.h"

namespace Shard3D {

//=====================================================================================================================
//=====================================================================================================================

// Definitions are engine only, requires recompiling static library to take effect

const static Version ENGINE_VERSION = { VersionState::Alpha, 1, 2, 6, 5 };	// Shard3D 

#define ENGINE_SETTINGS_PATH						"assets/configdata/engine_settings.ini"
#define EDITOR_SETTINGS_PATH						"assets/configdata/editor_settings.ini"
#define GAME_SETTINGS_PATH							"assets/configdata/game_settings.ini"	

#define ENGINE_ERRTEX								"assets/_engine/tex/null_tex.png"	// purple checkerboard
#define ENGINE_ERRMTX								"assets/_engine/tex/null_mat.png"	// blank world grid texture

#define ENGINE_BLKTEX								"assets/_engine/tex/0x000000.png"	// black texture
#define ENGINE_WHTTEX								"assets/_engine/tex/0xffffff.png"	// white texture
#define ENGINE_NRMTEX								"assets/_engine/tex/0x807ffe.png"	// blank normal texture

#define ENGINE_ERRMSH								"assets/_engine/msh/null_mdl.obj"	// obj model

#define ENGINE_ERRMAT								"assets/_engine/mat/world_grid"		// opaque surface material (world grid)

#define ENGINE_ASSETS_PATH							"assets"
#define ENGINE_ASSET_SUFFIX							".s3dasset"
#define ENGINE_SHADER_FILES_PATH					"assets/shaderdata/"				//Shader files (SPIR-V)
#define ENGINE_DEFAULT_MODEL_FILE					"assets/_engine/msh/cube.obj"
#define ENGINE_CORE_ASSETS_PATH						"assets/_engine/"	
#define ENGINE_MONO_SCRIPT_ASSEMBLY_PATH			"assets/scriptdata"
#define ENGINE_CS_SCRIPT_RUNTIME_DLL				"assets/scriptdata/cs/export/shard3d-csscript-core64.dll"
#define ENGINE_VB_SCRIPT_RUNTIME_DLL				"assets/scriptdata/vb/export/shard3d-vbscript-core64.dll"
#define ENGINE_FONT_PATH							"assets/_engine/fnt/"		//Fonts
#define ENGINE_DEFAULT_ENGINE_FONT					"assets/_engine/fnt/fs-tahoma-8px.ttf"		//Default engine font
#define ENGINE_FONT_SIZE							16		//size in px
#define ENGINE_SHARD3D_LEVELFILE_OPTIONS			"Shard3D Level (*.s3dlevel)\0*.s3dlevel\0All files (*.*)\0*.*\0"
#define ENGINE_SHARD3D_ASSETFILE_OPTIONS			"Shard3D Asset (*.s3dasset)\0*.s3dasset\0All files (*.*)\0*.*\0"
#define ENGINE_SHARD3D_HUDFILE_OPTIONS				"Shard3D HUD (*.s3dhud)\0*.s3dhud\0All files (*.*)\0*.*\0"
#define ENGINE_MAX_POINTLIGHTS						128		// dont change because these dont match shader code
#define ENGINE_MAX_SPOTLIGHTS						128 
#define ENGINE_MAX_DIRECTIONAL_LIGHTS				6

//=====================================================================================================================
//=====================================================================================================================

// tools
#define ENSET_BETA_DEBUG_TOOLS		
//	#define ENSET_ENABLE_COMPUTE_SHADERS					// Compute shaders can be very useful, however as of now they have not been fully implemented, only enable for development

#ifdef _DEPLOY
#define SHARD3D_NO_ASSERT	// Disable assertions for slight performance boost
#endif
}