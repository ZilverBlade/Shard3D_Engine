#pragma once

#include "Layer.hpp"

#include "../systems/hud_system.hpp"
namespace Shard3D {
	class HUDLayer : public Shard3D::Layer {
	public:
		HUDLayer();
		~HUDLayer();

		void attach(VkRenderPass renderPass, LayerStack* layerStack) override;
		void detach() override;
		void update(FrameInfo& frameInfo) override;

		void addElement(std::shared_ptr<HUDElement> element);
		std::shared_ptr<HUDElement> getSelectedElement();
		void rmvElement(std::shared_ptr<HUDElement> element);
		void rmvElement(uint64_t id);
	private:
		bool contextRefresh;
		GLFWwindow* window;
		HUDRenderSystem hudRenderSystem;
		HUD hud;
		friend class EditorApp;
	};
}