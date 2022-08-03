#pragma once
#include "../hud.hpp"

namespace Shard3D {

	class HUDBuilderPanel {
	public:
		HUDBuilderPanel() = default;
		~HUDBuilderPanel();
		void setContext(std::shared_ptr<HUDContainer> gctnr);
		void render();
	private:
		void renderGUIBuilder();
		void renderGUIBuilderToolbar();
		void renderGUIElementProperties(std::shared_ptr<HUD::Element> element);
		std::shared_ptr<HUDContainer> hudLayerInfo;
		const int nodeFlags = 32 | 4;
	};
}
