#pragma once
#include "Layer.hpp"
#include "../pipeline.hpp"

#include <memory>
#include <vector>

namespace Shard3D {

	class ImGuiLayer : public Shard3D::Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void attach(VkRenderPass renderPass, EngineDevice* device, GLFWwindow* window) override;
		void detach() override;
		void update(VkCommandBuffer buffer, GLFWwindow* window, float dt) override;
	private:
		int width;
		int height;
		VkDescriptorPool descriptorPool;

		bool hasBeenDetached = false;

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

		EngineSettings enset;
		EditorPreferences edpref;
	};

}