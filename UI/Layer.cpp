#include "../s3dtpch.h"  
#include "Layer.hpp"

namespace Shard3D {

	Layer::Layer(const char* name) {}

	Layer::~Layer() {}

	void Layer::attach(VkRenderPass renderPass, LayerStack* layerStack) {}

	void Layer::detach(){}

	void Layer::update(FrameInfo& frameInfo) {}

}