#pragma once
#include "s3dtpch.h"
#include "texture.hpp"
namespace Shard3D {
	class GUI {
	public:
		struct Element {
			glm::vec2 position;
			glm::vec2 scale = {1.f, 1.f};
			std::string texturePath;
			std::string cachePath;
		};
		std::vector<Element> elementsGUI;
	};
}