#include "model.hpp"
#include "utils/engine_utils.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

//object loaders
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <tiny_gltf.h>

#include "simpleini/simple_ini.h"
#include "utils/definitions.hpp"
#include "engine_logger.hpp"

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

	std::unique_ptr<EngineModel> EngineModel::createModelFromFile(EngineDevice& device, const std::string& filepath, ModelType modelType, bool indexModel) {
		Builder builder{};	
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		isIndexed = indexModel;
		mType = modelType;
		fpath = filepath;
		
		std::ifstream f(filepath.c_str());
		if (!f.good()) { SHARD3D_ERROR("Invalid model, file '{0}' not found", filepath); return nullptr; };

		builder.loadIndexedModel(filepath, modelType);
		if (ini.GetBoolValue("LOGGING", "log.ModelLoadInfo") == true) {
			SHARD3D_INFO("Loaded model: '{0}'\n\t\tModel vertex count: {1}", filepath, builder.vertices.size());
		}	
		return std::make_unique<EngineModel>(device, builder);
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
		stagingBuffer.writeToBuffer((void*)vertices.data());

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
#pragma region GLTF
	void EngineModel::gltfInitialiseShit(const std::string& filepath) {

		std::ostringstream sstr;

		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			SHARD3D_FATAL("failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		sstr << file.rdbuf();
		file.close();

		JSON = json::parse(sstr.str());
		data = gltfGetData(filepath);
	}

	std::vector<unsigned char> EngineModel::gltfGetData(const std::string& filepath) {

		std::string bytesText;
		std::string uri = JSON["buffers"][0]["uri"];

		std::string fileDir = filepath.substr(0, filepath.find_last_of("/") + 1);

		std::ostringstream sstr;


		std::ifstream file{ fileDir + uri, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			SHARD3D_FATAL("failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		sstr << file.rdbuf();
		file.close();


		bytesText = sstr.str();
		std::vector<UCHAR> data(bytesText.begin(), bytesText.end());
		return data;
	}

	std::vector<float> EngineModel::gltfGetFloats(json accessor) {
		std::vector<float> floatVec;

		uint32_t buffViewInd = accessor.value("bufferView", 1);
		uint32_t count = accessor["count"];
		uint32_t accByteOffset = accessor.value("byteOffset", 0);
		std::string type = accessor["type"];

		json bufferView = JSON["bufferViews"][buffViewInd];
		uint32_t byteOffset = bufferView["byteOffset"];

		uint32_t numPerVert;
		if (type == "SCALAR")		numPerVert = 1;
		else if (type == "VEC2")	numPerVert = 2;
		else if (type == "VEC3")	numPerVert = 3;
		else if (type == "VEC4")	numPerVert = 4;
		else throw std::invalid_argument("Invalid type! Valid types are 'SCALAR', 'VEC2', 'VEC3' and 'VEC4'");

		uint32_t beginningOfData = byteOffset + accByteOffset;
		uint32_t lengthOfData = count * 4 * numPerVert;

		for (uint32_t i = beginningOfData; i < beginningOfData + lengthOfData; i++) {
			UCHAR bytes[] = { data[i++],data[i++] ,data[i++] ,data[i++] };
			float value;
			memcpy(&value, bytes, sizeof(float));
			floatVec.push_back(value);
		}

		return floatVec;
	}

	std::vector<GLuint> EngineModel::gltfGetIndices(json accessor) {
		std::vector<GLuint> indices;

		uint32_t buffViewInd = accessor.value("bufferView", 0);
		uint32_t count = accessor["count"];
		uint32_t accByteOffset = accessor.value("byteOffset", 0);
		uint32_t componentType = accessor["componentType"];

		json bufferView = JSON["bufferViews"][buffViewInd];
		uint32_t byteOffset = bufferView["byteOffset"];

		uint32_t beginningOfData = byteOffset + accByteOffset;
		if (componentType == 5125) {
			for (uint32_t i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i) {
				UCHAR bytes[] = { data[i++],data[i++] ,data[i++] ,data[i++] };
				uint32_t value;
				memcpy(&value, bytes, sizeof(uint32_t));
				indices.push_back((GLuint)value);
			}
		}
		else if (componentType == 5123) {
			for (uint32_t i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i) {
				UCHAR bytes[] = { data[i++],data[i++] };
				uint16_t value;
				memcpy(&value, bytes, sizeof(uint16_t));
				indices.push_back((GLuint)value);
			}
		}
		else if (componentType == 5123) {
			for (uint32_t i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i) {
				UCHAR bytes[] = { data[i++],data[i++] };
				short value;
				memcpy(&value, bytes, sizeof(short));
				indices.push_back((GLuint)value);
			}
		}
		return indices;
	}

	std::vector<glm::vec2> EngineModel::gltfGroupFloatsVec2(std::vector<float> floatVec) {
		std::vector<glm::vec2> vectors;
		for (int i = 0; i < floatVec.size(); i) {
			vectors.push_back(glm::vec2(floatVec[i++],floatVec[i++]));
		}
		return vectors;
	}
	std::vector<glm::vec3> EngineModel::gltfGroupFloatsVec3(std::vector<float> floatVec) {
		std::vector<glm::vec3> vectors;
		for (int i = 0; i < floatVec.size(); i) {
			vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
		}
		return vectors;
	}
	std::vector<glm::vec4> EngineModel::gltfGroupFloatsVec4(std::vector<float> floatVec) {
		std::vector<glm::vec4> vectors;
		for (int i = 0; i < floatVec.size(); i) {
			vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
		}
		return vectors;
	}
	std::vector<EngineModel::Vertex> EngineModel::assembleVertices(
		std::vector<glm::vec3> positions,
		std::vector<glm::vec3> normals,
		//std::vector<glm::vec3> colors,
		std::vector<glm::vec2> texCoords
	) {
		std::vector<Vertex> vertices;
		for (int i = 0; i < positions.size(); i++) {
			vertices.push_back(
				Vertex{
					{positions[i].x, -positions[i].z, positions[i].y},
					{normals[i].x, -normals[i].z, normals[i].y},
					glm::vec3(1.f, 1.f, 1.f),//colors[i],
					texCoords[i]
				}
			);
		}
		return vertices;
	}

	void EngineModel::loadGLTFMesh(uint32_t indMesh) {

		uint32_t posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
		uint32_t normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
		//uint32_t colorAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["COLOR"];
		uint32_t texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
		uint32_t indAccInd = JSON["meshes"][indMesh]["primitives"][0]["incidces"];

		std::vector<float> posVec = gltfGetFloats(JSON["accessors"][posAccInd]);
		std::vector<glm::vec3> positions = gltfGroupFloatsVec3(posVec);
		std::vector<float> normalVec = gltfGetFloats(JSON["accessors"][normalAccInd]);
		std::vector<glm::vec3> normals = gltfGroupFloatsVec3(normalVec);
		std::vector<float> texVec = gltfGetFloats(JSON["accessors"][texAccInd]);
		std::vector<glm::vec2> texUVs = gltfGroupFloatsVec2(texVec);

		std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
		std::vector<GLuint> indices = gltfGetIndices(JSON["accessors"][indAccInd]);
	}
#pragma endregion

	
	void EngineModel::Builder::loadIndexedModel(const std::string& filepath, ModelType modelType) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			SHARD3D_FATAL(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto &shape : shapes) {
			for (const auto &index : shape.mesh.indices) {
				Vertex vertex{};
				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index],		//X
						-attrib.vertices[3 * index.vertex_index + 2],	//Z, but is actually -Y
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
						-attrib.normals[3 * index.normal_index + 2],	//Z, but is actually -Y
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
	}

	void EngineModel::Builder::loadModel(const std::string& filepath, ModelType modelType) {
		if (modelType == ModelType::MODEL_TYPE_OBJ){
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn, err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
				SHARD3D_FATAL(warn + err);
			}

			vertices.clear();
			indices.clear();

			for (const auto& shape : shapes) {
				for (const auto& index : shape.mesh.indices) {
					Vertex vertex{};

					if (index.vertex_index >= 0) {
						vertex.position = {
							attrib.vertices[3 * index.vertex_index],		//X
							-attrib.vertices[3 * index.vertex_index + 2],	//Z, but is actually -Y
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
							-attrib.normals[3 * index.normal_index + 2],	//Z, but is actually -Y
							attrib.normals[3 * index.normal_index + 1],		//Y, but is actually Z
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
		else if (modelType == ModelType::MODEL_TYPE_GLTF) {
			//assert(indMesh != NULL && "Mesh index may not be 0 (GLTF loader)");
		}
	}
}