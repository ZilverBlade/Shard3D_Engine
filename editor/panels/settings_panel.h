#pragma once

#include <Shard3D/core/ecs/level.h>
#include <Shard3D/core/ecs/actor.h>
#include <filesystem>

using namespace Shard3D::ECS;
namespace Shard3D {
	class SettingsPanel {
	public:
		SettingsPanel();
		~SettingsPanel();

		void render();

		bool showEngineSettingsWindow = false;
		bool showEditorSettingsWindow = false;
		bool showGraphicsSettingsWindow = false;
	private:
		void renderEngineSettings();
		void renderEditorPreferences();
		void renderGraphicsSettings();

		struct EngineSettings {
			// WINDOW
			int DEFAULT_WIDTH{};
			int DEFAULT_HEIGHT{};
			bool Resizable{};
			char WindowName[64]{};

			// RENDERING
			int ViewCombo{};
			float NearClipDistance{};
			float FarClipDistance{};
			float FOV{};
			float defaultBGColor[3] = { 0.f, 0.f, 0.f };
			bool pbr{};
		};
		struct EditorPreferences {
			// WINDOW
			bool antiAliasedUI = true;
			//ImGuiColorEditFlags displayFloatOr255 = ImGuiColorEditFlags_Float;
		};

		EditorPreferences edpref;
		EngineSettings enset;

		int currentSelectedCategory{};
		
	};
}