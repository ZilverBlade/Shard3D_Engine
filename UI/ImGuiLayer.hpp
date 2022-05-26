#pragma once
#include "Layer.hpp"

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
	};

}