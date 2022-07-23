#pragma once

#include "texture.hpp"
#include "GUID.hpp"
#include <vector>
#include <unordered_map>
namespace Shard3D {
	static void no_call() {}
	class GUI {
	public:
		enum class ElementType {
			Button,
			Image,
			Text
		};
		struct Element {
			glm::vec2 position;
			glm::vec2 scale = {1.f, 1.f};
			float rotation;
			std::string texturePath;
			std::string cachePath;
			GUID guid;
			std::string tag = "Some kind of element";

			void (*hoverEventCallback)() = no_call;
			void (*pressEventCallback)() = no_call;
			void (*clickEventCallback)() = no_call;
		};
		
		std::unordered_map<uint64_t, std::shared_ptr<Element>> elementsGUI;
	};

	class GUIContainer {
	public:
		std::vector<GUI*>& getList() {
			return guiLayerList;
		}
		void merge(int layer, GUI object) {
			for (auto& getEl : object.elementsGUI) {
				guiLayerList.at(layer)->elementsGUI.try_emplace(getEl.first, getEl.second);
			}
		}
		void swap(int layer, GUI* object) {
			for (auto& getEl : object->elementsGUI) {
				guiLayerList.at(layer)->elementsGUI = object->elementsGUI;
			}
		}
		void merge(int layer, std::shared_ptr<GUI::Element> object) {
			guiLayerList.at(layer)->elementsGUI.try_emplace(object->guid, object);
		}
	private:
		std::vector<GUI*> guiLayerList;
		friend class ImGuiLayer;
	};
}