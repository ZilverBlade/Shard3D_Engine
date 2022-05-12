#pragma once

#include "shard_device.hpp"
#include "shard_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace shard {

	class ShardModel {

	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex &other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadIndexedModel(const std::string& filepath);
			void loadModel(const std::string& filepath);
		};

		ShardModel(ShardDevice &device, const ShardModel::Builder &builder);
		~ShardModel();

		ShardModel(const ShardModel&) = delete;
		ShardModel& operator=(const ShardModel&) = delete;
		
		static std::unique_ptr<ShardModel> createModelFromFile(ShardDevice& device, const std::string& filepath, bool indexModel = true);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		
	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t> &indices);

		ShardDevice& shardDevice;

		std::unique_ptr<ShardBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<ShardBuffer> indexBuffer;
		uint32_t indexCount;
	};
}