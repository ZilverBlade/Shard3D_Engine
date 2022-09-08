#pragma once

#include <Shard3D/layer/layer.h>
#include <Shard3D/layer/layer_stack.h>
	
#include <Shard3D/core/vulkan_api/pipeline.h>

// panels
#include "../panels/level_tree_panel.h"
#include "../panels/level_properties_panel.h"
#include "../panels/level_peekers.h"
#include "../panels/asset_explorer_panel.h"
#include "../panels/material_builder_panel.h" 
#include "../panels/hud_builder_panel.h"

#include <Shard3D/events/mouse_event.h>
#include <Shard3D/events/key_event.h>
#include "../panels/settings_panel.h"
#include "../panels/default_component_panels.h"

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

		bool handleWindowResize(FrameInfo& frameInfo);

		void renderViewport(FrameInfo& frameInfo);

		VkDescriptorSet viewportImage{};
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
		DefaultComponentPanels componentActorPanels;
		AssetExplorerPanel assetExplorerPanel;
		SettingsPanel settingsPanel;
		HUDBuilderPanel hudBuilder;
		MaterialBuilderPanel materialBuilder;

		bool refreshContext;
//Gizmo levelGizmo;

		// icons

		struct {
			VkDescriptorSet play, pause, stop, 
				pref, save, load,
				preview, layout, viewport, level, launchgame,
				settings,
				l_save, l_load;
			
		} icons{};


	};

}