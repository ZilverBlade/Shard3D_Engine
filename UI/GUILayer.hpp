#pragma once

#include "Layer.hpp"

#include "../systems/gui_system.hpp"
namespace Shard3D {
	class GUILayer : public Shard3D::Layer {
	public:
		GUILayer();
		~GUILayer();

		void attach(VkRenderPass renderPass, LayerStack* layerStack) override;
		void detach() override;
		void update(FrameInfo& frameInfo) override;

		void addElement(std::shared_ptr<GUI::Element>  element);
		uint64_t getSelectedID();
		std::shared_ptr<GUI::Element> getSelectedElement();
		void rmvElement(std::shared_ptr<GUI::Element> element);
		void rmvElement(uint64_t id);
	private:
		bool contextRefresh;
		GLFWwindow* window;
		GUIRenderSystem guiRenderSystem;
		GUI guiElements;
		friend class ImGuiLayer;
	};
}