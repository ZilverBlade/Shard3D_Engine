#pragma once

#include "shard_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace shard {

	class ShardModel {

	public:

		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};
		ShardModel(ShardDevice &device, const std::vector<Vertex> &vertices);
		~ShardModel();

		ShardModel(const ShardModel&) = delete;
		ShardModel& operator=(const ShardModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		
	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		ShardDevice& shardDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}