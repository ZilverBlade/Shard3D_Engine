#include "shard_model.hpp"
#include <cassert>
namespace shard {

	ShardModel::ShardModel(ShardDevice& device, const std::vector<Vertex>& vertices) : shardDevice{device} {
		createVertexBuffers(vertices);
	}
	ShardModel::~ShardModel() {
		vkDestroyBuffer(shardDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(shardDevice.device(), vertexBufferMemory, nullptr);
	}

	void ShardModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());

		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		shardDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		void* data;
		vkMapMemory(shardDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(shardDevice.device(), vertexBufferMemory);
	}


	void ShardModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}
	void ShardModel::draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	std::vector<VkVertexInputBindingDescription> ShardModel::Vertex::getBindingDescriptions() {
		return { {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX} };
	}

	std::vector<VkVertexInputAttributeDescription> ShardModel::Vertex::getAttributeDescriptions() {
		return { 
			{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)},	//position
			{1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)}		//colour
		};
	}


}