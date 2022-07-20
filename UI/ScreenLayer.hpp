#pragma once

#include "Layer.hpp"

#include "../systems/gui_system.hpp"
namespace Shard3D {
	class ScreenLayer : public Shard3D::Layer {
	public:
		ScreenLayer();
		~ScreenLayer();

		void attach(VkRenderPass renderPass, LayerStack* layerStack) override;
		void detach() override;
		void update(FrameInfo& frameInfo) override;
	private:
		GUIRenderSystem guiRenderSystem;
		VkDescriptorSet screenImage;
	};
}