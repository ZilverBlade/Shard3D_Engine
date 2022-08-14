#pragma once

#include "../asset/texture.h"
#include "../misc/UUID.h"
#include "../../s3dstd.h"
#include <glm/glm.hpp>
#include <vector>

namespace Shard3D {
	struct HUDElement {
		glm::vec2 position;
		glm::vec2 scale = { 0.2f, 0.2f };
		int zPos;
		float rotation;
		glm::vec2 anchorOffset;

		UUID guid;

		bool isActive;

		std::string scriptmodule;
		int scriptlang;

		std::string tag = "Some kind of HUD";

		std::string default_texture	= "assets/_engine/tex/null_tex.png";
		std::string hover_texture	= "assets/_engine/tex/null_tex.png";
		std::string press_texture	= "assets/_engine/tex/null_tex.png";
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
		hashMap<uint64_t, sPtr<HUDElement>> elements;
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
		void insert(int layer, sPtr<HUDElement> object) {
			hudLayerList.at(layer)->elements.try_emplace(object->guid, object);
		}
		void remove(int layer, sPtr<HUDElement> object) {
			hudLayerList.at(layer)->elements.erase(object->guid);
		}
		void wipe(int layer) {
			hudLayerList.at(layer)->elements.clear();
		}

	private:
		std::vector<HUD*> hudLayerList;
		friend class EngineApplication;
	};
}