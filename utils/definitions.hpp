#pragma once

namespace Shard3D{

#define ENGINE_VERSION				"1.0.5"//{1, 0, 5, nullptr} // major, minor, revision, build

#define ENGINE_SETTINGS_PATH		"settings/engine_settings.ini"
#define EDITOR_SETTINGS_PATH		"settings/editor_settings.ini"	//unused
#define GAME_SETTINGS_PATH			"settings/game_settings.ini"	

#define SHADER_FILES_PATH			"shaders/"					//Shader files (SPIR-V)
#define DEFAULT_MODEL_PATH			"engine-assets/modeldata/"	//Default engine models
#define FONT_PATH					"engine-assets/fonts/"		//Fonts
#define DEFAULT_ENGINE_FONT			"engine-assets/fonts/sui-generis-free.rg-regular.ttf"		//Default engine font

	//engine settings
#define MAX_POINTLIGHTS			20 //dont change because these dont match shader code
#define MAX_SPOTLIGHTS			20 
#define MAX_DIRECTIONAL_LIGHTS	6
}