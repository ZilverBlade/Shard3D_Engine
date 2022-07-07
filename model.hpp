#pragma once
#include "s3dtpch.h"

#include "device.hpp"
#include "buffer.hpp"

#include <json.hpp>
#include "systems/material_system.hpp"

using json = nlohmann::json;

namespace Shard3D {

	enum class ModelType {
		MODEL_TYPE_NULL = 0,

		MODEL_TYPE_OBJ = 1,
		MODEL_TYPE_COLLADA = 2,
		MODEL_TYPE_GLTF = 3,
		MODEL_TYPE_FBX = 4
	};

	class EngineModel {

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

			void loadIndexedModel(const std::string& filepath, ModelType modelType);
		};

		inline static std::string fpath;
		inline static ModelType mType;

		EngineModel(const EngineModel::Builder &builder);
		~EngineModel();

		EngineModel(const EngineModel&) = delete;
		EngineModel& operator=(const EngineModel&) = delete;
		
		static std::unique_ptr<EngineModel> createModelFromFile(
			const std::string& filepath, 
			ModelType modelType
		);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		std::string getFile() { return fpath; }
		ModelType getType() { return mType; }
	private:
		std::unordered_map<std::string, MaterialSystem::Material> primitivesMaterialList; // <primitive material name, material>

		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t> &indices);

		EngineDevice& engineDevice;

		std::unique_ptr<EngineBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<EngineBuffer> indexBuffer;
		uint32_t indexCount;

#pragma region GLTF
		json JSON;
		std::vector<unsigned char> data;
		void gltfInitialiseShit(const std::string& filepath);
		std::vector<unsigned char> gltfGetData(const std::string& filepath);
		std::vector<float> gltfGetFloats(json accessor);
		std::vector<GLuint> gltfGetIndices(json accessor);
		
		std::vector<Vertex> assembleVertices(
			std::vector<glm::vec3> positions,
			std::vector<glm::vec3> normals,
			//std::vector<glm::vec3> colors,
			std::vector<glm::vec2> texCoords
		);

		std::vector<glm::vec2> gltfGroupFloatsVec2(std::vector<float> floatVec);
		std::vector<glm::vec3> gltfGroupFloatsVec3(std::vector<float> floatVec);
		std::vector<glm::vec4> gltfGroupFloatsVec4(std::vector<float> floatVec);
		void loadGLTFMesh(uint32_t indMesh);
#pragma endregion	
	};
}
