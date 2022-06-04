#pragma once

namespace Shard3D{

#define ENABLE_WORLDBUILDER3D		true;

#define ENGINE_VERSION				"1.0.7"	// Shard3D {major, minor, revision, build}
#define EDITOR_VERSION				"0.1"	// WorldEditor3D{major, minor}

#define ENGINE_SETTINGS_PATH		"settings/engine_settings.ini"
#define EDITOR_SETTINGS_PATH		"settings/editor_settings.ini"	//unused
#define GAME_SETTINGS_PATH			"settings/game_settings.ini"	

#define SHADER_FILES_PATH			"shaders/"					//Shader files (SPIR-V)
#define DEFAULT_MODEL_PATH			"engine-assets/modeldata/"	//Default engine models
#define FONT_PATH					"engine-assets/fonts/"		//Fonts
#define DEFAULT_ENGINE_FONT			"engine-assets/fonts/sui-generis-free.rg-regular.ttf"		//Default engine font
#define WINDOW_ICON_PATH			"shardwinico.png"

	//engine settings
#define MAX_POINTLIGHTS						20 //dont change because these dont match shader code
#define MAX_SPOTLIGHTS						20 
#define MAX_DIRECTIONAL_LIGHTS				6

#define ACTOR_FORCE_TRANSFORM_COMPONENT		true; // creates transform component automatically for all actors

}