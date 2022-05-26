#include "model.hpp"
#include "utils/engine_utils.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>>

//object loaders
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "simpleini/simple_ini.h"
#include "utils/definitions.hpp"

namespace std {
	template <>
	struct hash<Shard3D::EngineModel::Vertex> {
		size_t operator()(Shard3D::EngineModel::Vertex const& vertex) const {
			size_t seed = 0;
			Shard3D::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Shard3D {

	EngineModel::EngineModel(EngineDevice& device, const EngineModel::Builder &builder) : engineDevice{device} {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}
	EngineModel::~EngineModel() {}

	std::unique_ptr<EngineModel> EngineModel::createModelFromFile(EngineDevice& device, const std::string& filepath, bool indexModel) {
		Builder builder{};	
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		if (indexModel) {
			builder.loadIndexedModel(filepath);
			if (ini.GetBoolValue("LOGGING", "log.ModelLoadInfo") == true) {
				std::cout << "Loaded model: " << filepath << "\n" << "Model vertex count: " << builder.vertices.size() << "\n";
			}
			return std::make_unique<EngineModel>(device, builder);
		}
		else {
			builder.loadModel(filepath);
			if (ini.GetBoolValue("LOGGING", "log.ModelLoadInfo") == true) {
				std::cout << "Loaded model: " << filepath << "\n" << "Model vertex count: " << builder.vertices.size() << " (higher vertex count due to no indexing)\n";
			}
			return std::make_unique<EngineModel>(device, builder);
		}	
	}

	void EngineModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = (uint32_t)(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		EngineBuffer stagingBuffer{
			engineDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void *)vertices.data());

		vertexBuffer = std::make_unique<EngineBuffer>(
			engineDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		engineDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}

	void EngineModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
		indexCount = (uint32_t)(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer) { return; }
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		EngineBuffer stagingBuffer{
			engineDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());
	
		indexBuffer = std::make_unique<EngineBuffer>(
			engineDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		engineDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	void EngineModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void EngineModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	std::vector<VkVertexInputBindingDescription> EngineModel::Vertex::getBindingDescriptions() {
		return { {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX} };
	}

	std::vector<VkVertexInputAttributeDescription> EngineModel::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	void EngineModel::Builder::loadIndexedModel(const std::string& filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto &shape : shapes) {
			for (const auto &index : shape.mesh.indices) {
				Vertex vertex{};
				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};														
					vertex.color = {
						attrib.colors[3 * index.vertex_index],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};		
				}
				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}
				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

	void EngineModel::Builder::loadModel(const std::string& filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					auto colorIndex = 3 * index.vertex_index + 2;
					if (colorIndex < attrib.colors.size()) {
						vertex.color = {
							attrib.colors[colorIndex - 2],
							attrib.colors[colorIndex - 1],
							attrib.colors[colorIndex],
						};
					}
				}
				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}
				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}
				vertices.push_back(vertex);
			}
		}
	}
}