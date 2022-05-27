#pragma once

namespace Shard3D{

#define ENGINE_SETTINGS_PATH		"settings/engine_settings.ini"
#define EDITOR_SETTINGS_PATH		"settings/editor_settings.ini" //this one is unused
#define GAME_SETTINGS_PATH			"settings/game_settings.ini"

#define SHADER_FILES_PATH			"shaders/"
#define DEFAULT_MODEL_PATH			"modeldata/"

	//engine settings
#define MAX_POINTLIGHTS			20 //dont change because these dont match shader code
#define MAX_SPOTLIGHTS			20 
#define MAX_DIRECTIONAL_LIGHTS	6
}