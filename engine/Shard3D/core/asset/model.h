#pragma once
#include "../../vulkan_abstr.h"

namespace Shard3D {
	enum class MeshType {
		MESH_TYPE_NULL = 0,
		
		MESH_TYPE_OBJ = 1,
		MESH_TYPE_COLLADA = 2,
		MESH_TYPE_GLTF = 3,
		MESH_TYPE_FBX = 4
	};

	class EngineMesh {
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

			void loadIndexedMesh(const std::string& filepath, MeshType modelType);
		};

		EngineMesh(EngineDevice& dvc, const EngineMesh::Builder &builder);
		~EngineMesh();

		EngineMesh(const EngineMesh&) = delete;
		EngineMesh& operator=(const EngineMesh&) = delete;
		
		static uPtr<EngineMesh> createMeshFromFile(
			EngineDevice& dvc,
			const std::string& filepath, 
			MeshType modelType
		);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t> &indices);

		uPtr<EngineBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		uPtr<EngineBuffer> indexBuffer;
		uint32_t indexCount;
		EngineDevice* device;
	};
}
