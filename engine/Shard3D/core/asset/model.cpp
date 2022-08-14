#include "../../s3dpch.h" 
#include <glm/gtx/hash.hpp>

//object loaders
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "model.h"

#include "../../core.h"
#include "../../ecs.h"

namespace std {
	template <>
	struct hash<Shard3D::EngineMesh::Vertex> {
		size_t operator()(Shard3D::EngineMesh::Vertex const& vertex) const {
			size_t seed = 0;
			Shard3D::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Shard3D {

	EngineMesh::EngineMesh(EngineDevice& dvc, const EngineMesh::Builder &builder) : device(&dvc) {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}
	EngineMesh::~EngineMesh() {}

	uPtr<EngineMesh> EngineMesh::createMeshFromFile(EngineDevice& dvc, const std::string& filepath, MeshType modelType) {
		Builder builder{};	
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		
		std::ifstream f(filepath.c_str());
		if (!f.good()) { SHARD3D_ERROR("Invalid model, file '{0}' not found", filepath); return uPtr<EngineMesh>(nullptr); };

		builder.loadIndexedMesh(filepath, modelType);
		if (ini.GetBoolValue("LOGGING", "log.MeshLoadInfo") == true) {
			SHARD3D_INFO("Loaded model: '{0}'\n\t\tMesh vertex count: {1}", filepath, builder.vertices.size());
		}	
		return make_uPtr<EngineMesh>(dvc, builder);
	}

	void EngineMesh::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = (uint32_t)(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		EngineBuffer stagingBuffer{
			*device,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = make_uPtr<EngineBuffer>(
			*device,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		device->copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}

	void EngineMesh::createIndexBuffers(const std::vector<uint32_t> &indices) {
		indexCount = (uint32_t)(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer) { return; }
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		EngineBuffer stagingBuffer{
			*device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());
	
		indexBuffer = make_uPtr<EngineBuffer>(
			*device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		device->copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	void EngineMesh::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void EngineMesh::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	std::vector<VkVertexInputBindingDescription> EngineMesh::Vertex::getBindingDescriptions() {
		return { {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX} };
	}
	
	std::vector<VkVertexInputAttributeDescription> EngineMesh::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.reserve(4); 
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)	});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)		});
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)	});
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT,	offsetof(Vertex, uv)		});
		return attributeDescriptions;
	}
	
	void EngineMesh::Builder::loadIndexedMesh(const std::string& filepath, MeshType modelType) {
		auto newTime = std::chrono::high_resolution_clock::now();

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			SHARD3D_FATAL(warn + err);
		}
		
		vertices.clear();
		indices.clear();

		hashMap<Vertex, uint32_t> uniqueVertices{};
		for (const auto &shape : shapes) {
			indices.reserve(shape.mesh.indices.size()); //preallocate
			for (const auto &index : shape.mesh.indices) {
				Vertex vertex{};
				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index],		//X
						attrib.vertices[3 * index.vertex_index + 2],	//Z, but is actually Y
						attrib.vertices[3 * index.vertex_index + 1], 	//Y, but is actually Z		
					};													
					vertex.color = {
						attrib.colors[3 * index.vertex_index],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};		
				}
				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index],			//X
						attrib.normals[3 * index.normal_index + 2],		//Z, but is actually Y
						attrib.normals[3 * index.normal_index + 1],		//Y, but is actually Z
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
		SHARD3D_LOG("Duration of loading vertices: {0} ms", 
			std::chrono::duration<float, std::chrono::milliseconds::period>
			(std::chrono::high_resolution_clock::now() - newTime).count());
	}
}