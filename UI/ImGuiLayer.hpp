#pragma once
#include "Layer.hpp"
#include "../pipeline.hpp"

#include <imgui_node_editor.h>
#include <memory>
#include <vector>
#include "../worldbuilder3d_imgui_frame.hpp"
namespace Shard3D {

	class ImGuiLayer : public Shard3D::Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void attach(VkRenderPass renderPass, EngineDevice* device, GLFWwindow* window) override;
		void detach() override;
		void update(VkCommandBuffer buffer, GLFWwindow* window, float dt) override;

        void pushError(const char* message);
	private:
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

		WorldBuilder3D::WorldBuilder3DConsole console;
	};

}