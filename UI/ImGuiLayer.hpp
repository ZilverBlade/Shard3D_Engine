#pragma once
#include "Layer.hpp"
#include "../pipeline.hpp"
#include "LayerStack.hpp"
#include <imgui_node_editor.h>

// panels
#include "level_tree_panel.hpp"
#include "level_properties_panel.hpp"
#include "level_peekers.hpp"
#include "asset_explorer_panel.hpp"
#include "level_gizmo.hpp"
#include "hud_builder_panel.hpp"
namespace Shard3D {
	class HUDLayer;
	class HUD;
	class ImGuiLayer : public Shard3D::Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void attach(VkRenderPass renderPass, LayerStack* layerStack) override;
		void detach() override;
		void update(FrameInfo& frameInfo) override;
		void attachGUIEditorInfo(HUDLayer** guiArray);
	private:
		void detachTag() { }
		void createIcons();
		void renderQuickBar();
		void renderMenuBar();
		int width;
		int height;

		LayerStack* currentStack;

		std::shared_ptr<HUDContainer> guiLayers{};

		ax::NodeEditor::EditorContext* nodeEditorContext;

		bool hasBeenDetached = false;
		float timeSinceLastSecond;
		float deltaTimeFromLastSecond;

		bool showTest = false;
		bool showStatsWindow = false;
		bool showEngineSettingsWindow = false;
		bool showGraphicsSettingsWindow = false;
		bool showStylizersWindow = false;
		bool showCredits = false;
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
			bool pbr{};
		};
		struct EditorPreferences {
			// WINDOW
			bool antiAliasedUI = true;
			ImGuiColorEditFlags displayFloatOr255 = ImGuiColorEditFlags_Float;
		};

		EditorPreferences edpref;
		EngineSettings enset;

		LevelTreePanel levelTreePanel;
		LevelPropertiesPanel levelPropertiesPanel;
		LevelPeekingPanel levelPeekPanel;
		AssetExplorerPanel AssetExplorerPanel;
		HUDBuilderPanel guiBuilder;
		bool refreshContext;
//Gizmo levelGizmo;

		// icons

		struct {
			VkDescriptorSet play, pause, stop, 
				pref, save, load,
				preview, layout, viewport, level,
				settings,
				l_save, l_load;
			
		} icons;
	};

}