#include "Layer.hpp"

namespace Shard3D {

	Layer::Layer(const char* name) {}

	Layer::~Layer() {}

	void Layer::attach(){}

	void Layer::attach(VkRenderPass renderPass, EngineDevice* device, GLFWwindow* window) {}

	void Layer::detach(){}

	void Layer::update(VkCommandBuffer buffer, GLFWwindow* window, float dt, std::shared_ptr<wb3d::Level>& level) {}

}