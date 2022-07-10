#pragma once

namespace Shard3D {

//=====================================================================================================================
//=====================================================================================================================

const static std::string ENGINE_VERSION = "1.1.4.r2-alpha";	// Shard3D {major, minor, patch, revision}-state
const static std::string EDITOR_VERSION = "1.1.r1-alpha";	// WorldEditor3D {major, minor};

#define ENGINE_SETTINGS_PATH		"assets/settings/engine_settings.ini"
#define EDITOR_SETTINGS_PATH		"assets/settings/editor_settings.ini"	//unused
#define GAME_SETTINGS_PATH			"assets/settings/game_settings.ini"	

// DONT TOUCH
#define ENGINE_ERRTEX				"assets/_engine/tex/null_tex.png" // png texture
#define ENGINE_ERRMAT				"assets/_engine/tex/null_mat.png" // png texture
#define ENGINE_ERRMSH				"assets/_engine/msh/null_mdl.obj" // obj model
#define ENGINE_ASSETS_PATH					"assets/"
#define ENGINE_SHADER_FILES_PATH			"assets/shaders/"			//Shader files (SPIR-V)
#define ENGINE_DEFAULT_MODEL_FILE	"assets/_engine/msh/cube.obj"
#define ENGINE_ASSETS_PATH			"assets/_engine/"	
#define ENGINE_FONT_PATH					"assets/_engine/fnt/"		//Fonts
#define ENGINE_DEFAULT_ENGINE_FONT			"assets/_engine/fnt/tahoma.ttf"		//Default engine font
#define ENGINE_FONT_SIZE			16		//size in px
#define ENGINE_WORLDBUILDER3D_FILE_OPTIONS	"WorldBuilder3D Level (*.wbl)\0*.wbl\0All files (*.*)\0*.*\0"
#define ENGINE_WORLDBUILDER3D_ASSETFILE_OPTIONS "WorldBuilder3D Asset (*.wbasset)\0*.wbasset\0All files (*.*)\0*.*\0"
#define ENGINE_MAX_POINTLIGHTS						20		// dont change because these dont match shader code
#define ENGINE_MAX_SPOTLIGHTS						20 
#define ENGINE_MAX_DIRECTIONAL_LIGHTS				6

//=====================================================================================================================
//=====================================================================================================================


// shipping settings
#ifndef _DEPLOY
#define ENSET_ENABLE_WORLDBUILDER3D	true
#endif
#define ENSET_BETA_DEBUG_TOOLS		true


// experimentals
#define ENSET_USE_PBR								false	// PBR can look better, but it's experimental as of now and it's significantly slower and buggier;
#define ENSET_ENABLE_COMPUTE_SHADERS				false	// Compute shaders can be very useful, however as of now they have not been fully implemented, only enable for development
#ifndef _DEPLOY
#define ENSET_ALLOW_PREVIEW_CAMERA					false	// Allow previewing camera, has impact on performance and is broken at the moment. 
#endif

// engine settings


#define ENSET_WINDOW_ICON_PATH			"shardwinico.png"

#define ENSET_WB3DLEVEL_CIPHER_KEY			128	// for level encyption and decryption for release ready games, MUST BE A MULTIPLE OF 2!!

#define ENSET_ENABLE_PLUGINS						true
#define ENSET_ACTOR_FORCE_TRANSFORM_COMPONENT		true	// creates transform component automatically for all actors, recommended to keep enabled as meshes heavily rely on this
#define ENSET_MAX_ACTOR_CHILDREN_COUNT				256

#define ENSET_CONFIDENT_COMPONENTS					false	// when set to true, no checks will be done to see if component exists. This can speed things up as it removes the if (exists) check, however will cause a crash if no component was found. Keep this on while developing, to catch mistakes.
#define ENSET_CONFIDENT_ASSETS						false	// when set to true, no default texture or model will be returned if no asset was found. This can speed things up as it removes the if (exists) check, however will cause a crash if no asset was found.
#define ENSET_DUPLICATE_ASSETS_ON_FIND_FAIL			false	// alternative to ENSET_CONFIDENT_ASSETS; rather than checking if the asset exists in the asset map, and crash if none were found, this will push through a default model or texture into the asset map if no file was found or stored. This is a preferred option for shipping as has the benefit of letting the user know that certain assets are not being loaded, while not getting the performance slowdown from if (exists) checks are making. Only (possibly negligeable) drawback is that it will cause duplicate assets in memory, filling it up. Hoewever the texture size is 147 bytes, so it shouldn't be a big issue.
}