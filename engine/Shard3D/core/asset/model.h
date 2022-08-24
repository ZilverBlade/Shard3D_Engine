#pragma once
#include "../../vulkan_abstr.h"
#include "material.h"
#include "../misc/assetid.h"
extern "C" {
	struct aiNode;
	struct aiScene;
	struct aiMesh;
}
namespace Shard3D {
	struct MeshLoadInfo {
		int x;
	};

	class EngineMesh {
	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex &other) const {
				return position == other.position && normal == other.normal && uv == other.uv;
			}
		};

		struct SubmeshData {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		struct SubmeshBuffers {
			sPtr<EngineBuffer> indexBuffer{};
			sPtr<EngineBuffer> vertexBuffer{};

			uint32_t vertexCount{};
			uint32_t indexCount{};
			bool hasIndexBuffer = false;
		};

		struct Builder {
			//    <Material Slot> <Submesh>
			hashMap<std::string, SubmeshData> submeshes;
			void loadScene(const std::string& filepath, bool createMaterials);
			void processNode(aiNode* node, const aiScene* scene, bool createMaterials);
			void loadSubmesh(aiMesh* mesh, const aiScene* scene, bool createMaterials);
		};

		EngineMesh(EngineDevice& dvc, const EngineMesh::Builder &builder);
		~EngineMesh();

		EngineMesh(const EngineMesh&) = delete;
		EngineMesh& operator=(const EngineMesh&) = delete;
		
		static uPtr<EngineMesh> createMeshFromFile(
			EngineDevice& dvc,
			const std::string& filepath, 
			MeshLoadInfo loadInfo
		);
		static uPtr<EngineMesh> loadMeshFromFile(
			EngineDevice& dvc,
			const AssetID& asset,
			MeshLoadInfo loadInfo
		);
		void bind(VkCommandBuffer commandBuffer, SubmeshBuffers buffers);
		void draw(VkCommandBuffer commandBuffer, SubmeshBuffers buffers);

		//			<Buffer Data>
		std::vector<SubmeshBuffers> buffers{};

	private:
		void createVertexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);
		void createIndexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);
		void createMaterialBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);


		EngineDevice* device;
	};
}
