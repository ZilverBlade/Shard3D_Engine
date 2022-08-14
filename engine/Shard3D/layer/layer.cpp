#include "../s3dpch.h"  
#include "layer.h"

namespace Shard3D {

	Layer::Layer(const char* name) {}

	Layer::~Layer() {}

	void Layer::attach(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, LayerStack* layerStack) {}

	void Layer::detach(){}

	void Layer::update(FrameInfo& frameInfo) {}

}