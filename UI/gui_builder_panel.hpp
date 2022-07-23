#pragma once
#include "../gui.hpp"

namespace Shard3D {

	class GuiBuilderPanel {
	public:
		GuiBuilderPanel() = default;
		~GuiBuilderPanel();
		void setContext(std::shared_ptr<GUIContainer> gctnr);
		void render();
	private:
		void renderGUIBuilder();
		void renderGUIBuilderToolbar();
		void renderGUIElementProperties(std::shared_ptr<GUI::Element> element);
		std::shared_ptr<GUIContainer> guiLayerInfo;
		const int nodeFlags = 32 | 4;
	};
}
