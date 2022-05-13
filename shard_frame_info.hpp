#pragma once

#include "camera.hpp"
#include "shard_game_object.hpp"

#include <vulkan/vulkan.h>

namespace shard {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		ShardCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		ShardGameObject::Map& gameObjects;
	};
}