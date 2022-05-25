#pragma once

#include "Layer.hpp"

namespace Shard3D {
	class TestLayer : public Shard3D::Layer {
	public:
		TestLayer();

		void update(VkCommandBuffer buffer, GLFWwindow* window, float dt) override;

	};
}