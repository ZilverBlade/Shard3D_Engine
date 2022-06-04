#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "../device.hpp"
#include "../utils/definitions.hpp"
#include <memory>
#include "../wb3d/level.hpp"

namespace Shard3D {
	class Layer {
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void attach();
		virtual void attach(VkRenderPass renderPass, EngineDevice* device, GLFWwindow* window, std::shared_ptr<wb3d::Level>& level);

		virtual void detach();

		virtual void update(VkCommandBuffer buffer, GLFWwindow* window, float dt, std::shared_ptr<wb3d::Level>& level);
		//virtual void event();

	private:

	};

}