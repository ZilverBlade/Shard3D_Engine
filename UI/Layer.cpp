#include "Layer.hpp"

namespace Shard3D {

	Layer::Layer(const char* name) {}

	Layer::~Layer() {}

	void Layer::attach(VkRenderPass renderPass) {}

	void Layer::detach(){}

	void Layer::update(VkCommandBuffer buffer, float dt) {}

}