#pragma once
#include <string>

namespace Shard3D {
#ifndef GAME_RELEASE_READY
#define ENABLE_WORLDBUILDER3D		true
#endif
#define BETA_DEBUG_TOOLS			true

const static std::string ENGINE_VERSION = "1.1.2.r0-alpha";	// Shard3D {major, minor, patch, revision}-state
const static std::string EDITOR_VERSION = "1.1.r0-alpha";	// WorldEditor3D {major, minor};

#define ENGINE_SETTINGS_PATH		"assets/settings/engine_settings.ini"
#define EDITOR_SETTINGS_PATH		"assets/settings/editor_settings.ini"	//unused
#define GAME_SETTINGS_PATH			"assets/settings/game_settings.ini"	

// DONT TOUCH
#define ASSETS_PATH					"assets/"
#define SHADER_FILES_PATH			"assets/shaders/"			//Shader files (SPIR-V)
#define DEFAULT_MODEL_FILE			"assets/modeldata/engineModels/cube.obj"
#define DEFAULT_MODEL_PATH			"assets/engine-assets/modeldata/"	//Default engine models
#define ENGINE_ASSETS_PATH			"assets/engine-assets/"	
#define FONT_PATH					"assets/engine-assets/fonts/"		//Fonts
#define DEFAULT_ENGINE_FONT			"assets/engine-assets/fonts/sui-generis-free.rg-regular.ttf"		//Default engine font
#define ENGINE_FONT_SIZE			16		//size in px

// experimentals
#define USE_PBR								false	// PBR can look better, but it's experimental as of now and it's significantly slower and buggier;
#define ENABLE_COMPUTE_SHADERS				false	// Compute shaders can be very useful, however as of now they have not been fully implemented, only enable for development

// engine settings
#define MAX_POINTLIGHTS						20		// dont change because these dont match shader code
#define MAX_SPOTLIGHTS						20 
#define MAX_DIRECTIONAL_LIGHTS				6

#define WINDOW_ICON_PATH			"shardwinico.png"

// WB3D settings
#define LEVEL_CIPHER_KEY			128	// for level encyption and decryption for release ready games, MUST BE A MULTIPLE OF 2!!
#define WORLDBUILDER3D_FILE_OPTIONS	"WorldBuilder3D Level (*.wbl)\0*.wbl\0All files (*.*)\0*.*\0"
#define WORLDBUILDER3D_ASSETFILE_OPTIONS "WorldBuilder3D Asset (*.wbasset)\0*.wbasset\0All files (*.*)\0*.*\0"

#define ENABLE_PLUGINS						true
#define ACTOR_FORCE_TRANSFORM_COMPONENT		true	// creates transform component automatically for all actors, recommended to keep enabled as meshes heavily rely on this
#define MAX_ACTOR_CHILDREN_COUNT			256

}