#include "Layer.hpp"

namespace Shard3D {

	Layer::Layer(const char* name) {}

	Layer::~Layer() {}

	void Layer::attach(){}

	void Layer::attach(VkRenderPass renderPass, GLFWwindow* window) {}

	void Layer::detach(){}

	void Layer::update(VkCommandBuffer buffer, GLFWwindow* window, float dt) {}

}