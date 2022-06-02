#pragma once

#include "device.hpp"
#include "buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <json.hpp>

using json = nlohmann::json;

namespace Shard3D {

	enum class ModelType {
		MODEL_TYPE_OBJ,
		MODEL_TYPE_GLTF,
		MODEL_TYPE_FBX,
		MODEL_TYPE_COLLADA
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
			void loadModel(const std::string& filepath, ModelType modelType);
		};

		EngineModel(EngineDevice &device, const EngineModel::Builder &builder);
		~EngineModel();

		EngineModel(const EngineModel&) = delete;
		EngineModel& operator=(const EngineModel&) = delete;
		
		static std::unique_ptr<EngineModel> createModelFromFile(EngineDevice& device, const std::string& filepath, ModelType modelType, bool indexModel = true);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
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
		std::vector<UCHAR> data;
		void gltfInitialiseShit(const std::string& filepath);
		std::vector<UCHAR> gltfGetData(const std::string& filepath);
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
