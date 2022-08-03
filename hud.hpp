#pragma once

#include "texture.hpp"
#include "GUID.hpp"
#include <vector>
#include <unordered_map>
namespace Shard3D {
	static void no_call() {}
	class HUD {
	public:
		enum class ElementType {
			Button,
			Image,
			Text
		};
		struct Element {
			glm::vec2 position;
			glm::vec2 scale = {1.f, 1.f};
			int zPos;
			float rotation;
			std::string texturePath;
			std::string cachePath;
			GUID guid;
			std::string tag = "Some kind of HUD";
			int anchorType;

			void (*hoverEventCallback)() = no_call;
			void (*pressEventCallback)() = no_call;
			void (*clickEventCallback)() = no_call;
		};
		
		std::unordered_map<uint64_t, std::shared_ptr<Element>> elements;
	};

	class HUDContainer {
	public:
		std::vector<HUD*>& getList() {
			return hudLayerList;
		}
		void merge(int layer, HUD object) {
			for (auto& getEl : object.elements) {
				hudLayerList.at(layer)->elements.try_emplace(getEl.first, getEl.second);
			}
		}
		void swap(int layer, HUD* object) {
			for (auto& getEl : object->elements) {
				hudLayerList.at(layer)->elements = object->elements;
			}
		}
		void merge(int layer, std::shared_ptr<HUD::Element> object) {
			hudLayerList.at(layer)->elements.try_emplace(object->guid, object);
		}
	private:
		std::vector<HUD*> hudLayerList;
		friend class ImGuiLayer;
	};
}