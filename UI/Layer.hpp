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

		virtual void attach(VkRenderPass renderPass);

		virtual void detach();

		virtual void update(VkCommandBuffer buffer, float dt);

	private:

	};

}