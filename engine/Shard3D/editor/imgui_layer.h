#pragma once
#include <imgui_node_editor.h>

#include "../layer/layer.h"
#include "../layer/layer_stack.h"

#include "../core/vulkan_api/pipeline.h"

// panels
#include "level_tree_panel.h"
#include "level_properties_panel.h"
#include "level_peekers.h"
#include "asset_explorer_panel.h"
#include "level_gizmo.h"
#include "hud_builder_panel.h"
#include "../events/mouse_event.h"
#include "../events/key_event.h"

namespace Shard3D {
	class HUDLayer;
	class HUD;
	class ImGuiLayer : public Shard3D::Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void attach(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, LayerStack* layerStack) override;
		void detach() override;
		void update(FrameInfo& frameInfo) override;
		void attachGUIEditorInfo(sPtr<HUDContainer>& container);
	private:
		void eventEvent(Events::Event& e);
		bool mouseButtonDownEvent(Events::MouseButtonDownEvent& e);
		bool mouseButtonReleaseEvent(Events::MouseButtonReleaseEvent& e);
		bool mouseHoverEvent(Events::MouseHoverEvent& e);
		bool mouseScrollEvent(Events::MouseScrollEvent& e);
		bool keyDownEvent(Events::KeyDownEvent& e);
		bool keyReleaseEvent(Events::KeyReleaseEvent& e);
		bool keyPressEvent(Events::KeyPressEvent& e);
	private:
		void detachTag() { }
		void createIcons();
		void renderQuickBar(FrameInfo& frameInfo);
		void renderMenuBar(FrameInfo& frameInfo);
		int width;
		int height;

		LayerStack* currentStack;

		EngineWindow* engineWindow;
		EngineDevice* engineDevice;

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
		bool showDemoWindow = false;
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
		HUDBuilderPanel hudBuilder;
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