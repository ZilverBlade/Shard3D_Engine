#pragma once

#include "../../layer/layer.h"

#include "../../systems/rendering/hud_system.h"
#include "../../events/mouse_event.h"
namespace Shard3D {
	class HUDLayer : public Shard3D::Layer {
	public:
		HUDLayer();
		~HUDLayer();

		void attach(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, LayerStack* layerStack) override;
		void detach() override;
		void update(FrameInfo& frameInfo) override;

		void addElement(sPtr<HUDElement> element);
		sPtr<HUDElement> getSelectedElement();
		void rmvElement(sPtr<HUDElement> element);
		void rmvElement(uint64_t id);

		int layer;
	private:
		void eventEvent(Events::Event& e);
		bool mouseButtonDownEvent(Events::MouseButtonDownEvent& e);
		bool contextRefresh;
		HUDRenderSystem hudRenderSystem;
		HUD hud;
		friend class EditorApplication;
	};
}