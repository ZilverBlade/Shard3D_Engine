#pragma once
#include "../core.h"

#include "../core/misc/frame_info.h"
namespace Shard3D {
	class LayerStack;
	class Layer {
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void attach(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, LayerStack* layerStack);

		virtual void detach();

		virtual void update(FrameInfo& frameInfo);

	private:

	};

}