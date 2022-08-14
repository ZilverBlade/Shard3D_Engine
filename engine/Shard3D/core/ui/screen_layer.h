#pragma once

#include "../../layer/layer.h"
#include "../../systems/rendering/hud_system.h"

namespace Shard3D {
	class ScreenLayer : public Shard3D::Layer {
	public:
		ScreenLayer();
		~ScreenLayer();

		void attach(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, LayerStack* layerStack) override;
		void detach() override;
		void update(FrameInfo& frameInfo) override;
	private:
		HUDRenderSystem guiRenderSystem;
		VkDescriptorSet screenImage;
	};
}