#pragma once
#include "Layer.hpp"
#include "../pipeline.hpp"

#include <imgui_node_editor.h>
#include <memory>
#include <vector>
#include "../wb3d/wb3d_imgui_frame.hpp"

// panels
#include "level_tree_panel.hpp"
#include "level_properties_panel.hpp"
#include "level_peekers.hpp"
#include "level_gizmo.hpp"
namespace Shard3D {

	class ImGuiLayer : public Shard3D::Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void attach(VkRenderPass renderPass) override;
		void detach() override;
		void update(VkCommandBuffer buffer, float dt) override;

        void pushError(const char* message);
	private:
		void renderMenuBar();
		int width;
		int height;
		VkDescriptorPool descriptorPool;

		ax::NodeEditor::EditorContext* nodeEditorContext;

		bool hasBeenDetached = false;
		float timeSinceLastSecond;
		float deltaTimeFromLastSecond;

		bool showTest = false;
		bool showStatsWindow = false;
		bool showEngineSettingsWindow = false;
		bool showGraphicsSettingsWindow = false;

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
			float defaultBGColor[3] = {0.f, 0.f, 0.f};
		};
		struct EditorPreferences {
			// WINDOW
			bool antiAliasedUI = true;
			ImGuiColorEditFlags displayFloatOr255 = ImGuiColorEditFlags_Float;
		};

		EditorPreferences edpref;
		EngineSettings enset;
		wb3d::Console console;

		LevelTreePanel levelTreePanel;
		LevelPropertiesPanel levelPropertiesPanel;
		LevelPeekingPanel levelPeekPanel;

//Gizmo levelGizmo;
	};

}