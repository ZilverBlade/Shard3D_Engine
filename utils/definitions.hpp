#pragma once

namespace Shard3D{

#define ENABLE_WORLDBUILDER3D		true
#define GAME_RELEASE_READY			false // ONLY ENABLE WHEN READY FOR SHIPPING OR BETA TESTING! 
#define BETA_DEBUG_TOOLS			true

#define ENGINE_VERSION				"1.0.8"	// Shard3D {major, minor, revision, build}
#define EDITOR_VERSION				"0.1"	// WorldEditor3D{major, minor}

#define ENGINE_SETTINGS_PATH		"assets/settings/engine_settings.ini"
#define EDITOR_SETTINGS_PATH		"assets/settings/editor_settings.ini"	//unused
#define GAME_SETTINGS_PATH			"assets/settings/game_settings.ini"	

#define LEVEL_CIPHER_KEY			126	// for level encyption and decryption for release ready games, MUST BE A MULTIPLE OF 2!!
#define WORLDBUILDER3D_FILE_OPTIONS	"WorldBuilder3D Level (*.wbl)\0*.wbl\0All files (*.*)\0*.*\0"

#define ASSETS_PATH					"assets/"
#define SHADER_FILES_PATH			"assets/shaders/"			//Shader files (SPIR-V)
#define DEFAULT_MODEL_FILE			"assets/modeldata/engineModels/cube.obj"
#define DEFAULT_MODEL_PATH			"assets/engine-assets/modeldata/"	//Default engine models
#define FONT_PATH					"assets/engine-assets/fonts/"		//Fonts
#define DEFAULT_ENGINE_FONT			"assets/engine-assets/fonts/sui-generis-free.rg-regular.ttf"		//Default engine font
#define ENGINE_FONT_SIZE			16		//size in px
#define WINDOW_ICON_PATH			"shardwinico.png"

	//engine settings
#define MAX_POINTLIGHTS						20 //dont change because these dont match shader code
#define MAX_SPOTLIGHTS						20 
#define MAX_DIRECTIONAL_LIGHTS				6

#define ACTOR_FORCE_TRANSFORM_COMPONENT		true // creates transform component automatically for all actors, recommended to keep enabled as meshes heavily rely on this
#define MAX_ACTOR_CHILDREN_COUNT			256

}