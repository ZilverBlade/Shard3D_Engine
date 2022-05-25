#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Shard3D {
	class Layer {
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void attach();
		virtual void attach(VkRenderPass renderPass, GLFWwindow* window);

		virtual void detach();

		virtual void update(VkCommandBuffer buffer, GLFWwindow* window, float dt);
		//virtual void event();

	private:

	};

}