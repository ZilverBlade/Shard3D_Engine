#pragma once
#include "../core/ui/hud.h"

namespace Shard3D {

	class HUDBuilderPanel {
	public:
		HUDBuilderPanel() = default;
		~HUDBuilderPanel();

		void setContext(sPtr<HUDContainer> gctnr);
		void render();
	private:
		void cleanup();
		void renderGUIBuilder();
		void renderGUIBuilderToolbar();
		void renderGUIElementProperties(sPtr<HUDElement> element, int layer);
		sPtr<HUDContainer> hudLayerInfo;
		const int nodeFlags = 32 | 4;
	};
}
