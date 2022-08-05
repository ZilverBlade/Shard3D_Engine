#pragma once

#include "texture.hpp"
#include "GUID.hpp"
#include <vector>
#include <unordered_map>
namespace Shard3D {
	struct HUDElement {
		glm::vec2 position;
		glm::vec2 scale = { 0.2f, 0.2f };
		int zPos;
		float rotation;
		glm::vec2 anchorOffset;

		GUID guid;

		bool isActive;

		std::string scriptmodule;
		int scriptlang;

		std::string tag = "Some kind of HUD";

		std::string default_texture;
		std::string hover_texture;
		std::string press_texture;
	private:
		std::string _design_tex;
		friend class HUDRenderSystem;
		friend class HUDLayer;
	};
	class HUD {
	public:
		enum class ElementType {
			Button,
			Image,
			Text
		};
		std::unordered_map<uint64_t, std::shared_ptr<HUDElement>> elements;
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
		void merge(int layer, std::shared_ptr<HUDElement> object) {
			hudLayerList.at(layer)->elements.try_emplace(object->guid, object);
		}
		void wipe(int layer) {
			hudLayerList.at(layer)->elements.clear();
		}
	private:
		std::vector<HUD*> hudLayerList;
		friend class EditorApp;
	};
}