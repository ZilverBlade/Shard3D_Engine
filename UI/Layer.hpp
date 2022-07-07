#pragma once
#include "../s3dtpch.h"
#include "../device.hpp"
#include "../utils/definitions.hpp"

#include "../wb3d/level.hpp"

namespace Shard3D {
	class Layer {
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void attach(VkRenderPass renderPass);

		virtual void detach();

		virtual void update(VkCommandBuffer buffer, float dt);

	private:

	};

}