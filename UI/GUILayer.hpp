#pragma once
#include "../s3dtpch.h"
#include "Layer.hpp"

#include "../systems/gui_system.hpp"
namespace Shard3D {
	class GUILayer : public Shard3D::Layer {
	public:
		GUILayer();
		~GUILayer();

		void attach(VkRenderPass renderPass) override;
		void detach() override;
		void update(FrameInfo frameInfo) override;

		void addElement(GUI::Element element);
		void rmvElement(GUI::Element element);
		void rmvElement(size_t index);
	private:
		GUIRenderSystem guiRenderSystem;
		GUI guiElements;
	};
}