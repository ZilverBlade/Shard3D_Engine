#pragma once

#include "texture.hpp"
namespace Shard3D {
	class GUI {
		static void null_func() {}
	public:
		struct Element {
			glm::vec2 position;
			glm::vec2 scale = {1.f, 1.f};
			float rotation;
			std::string texturePath;
			std::string cachePath;
			uint64_t guid = 1;
			void (*hoverEventCallback)() = null_func;
			void (*pressEventCallback)() = null_func;
			void (*clickEventCallback)() = null_func;
		};
		std::unordered_map<uint64_t, std::shared_ptr<Element>> elementsGUI;
	};
}