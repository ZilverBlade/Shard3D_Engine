#pragma once
#include "../../vulkan_abstr.h"
#include "material.h"
#include "../asset/assetid.h"
extern "C" {
	struct aiNode;
	struct aiScene;
	struct aiMesh;
}
namespace Shard3D {
	inline namespace Resources {
		struct Model3DLoadInfo {
			int x;
		};
		class Model3D {
		public:
			struct Vertex {
				glm::vec3 position{};
				glm::vec3 normal{};
				glm::vec2 uv{};

				static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
				static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

				bool operator==(const Vertex& other) const {
					return position == other.position && normal == other.normal && uv == other.uv;
				}
			};

			struct SubmeshData {
				std::vector<Vertex> vertices{};
				std::vector<uint32_t> indices{};
				AssetID materialAsset{ "" };
			};

			struct SubmeshBuffers {
				sPtr<EngineBuffer> indexBuffer{};
				sPtr<EngineBuffer> vertexBuffer{};

				std::vector<uint32_t> vertexCounts{};
				std::vector<uint32_t> indexCounts{};
				std::vector<uint32_t> vertexOffsets{};
				std::vector<uint32_t> indexOffsets{};

				bool hasIndexBuffer = false;
			};

			struct Builder {
				//		<Material Slot>	 <Submesh>
				std::map<std::string, SubmeshData> submeshes;
				std::vector<SubmeshData*> submeshPtrs;
				std::vector<std::string> slotPtrs;
				void loadScene(const std::string& filepath, bool createMaterials);
				void processNode(aiNode* node, const aiScene* scene, bool createMaterials);
				void loadSubmesh(aiMesh* mesh, const aiScene* scene, bool createMaterials);

				std::string workingDir;
			};

			Model3D(EngineDevice& dvc, const Model3D::Builder& builder);
			~Model3D();

			Model3D(const Model3D&) = delete;
			Model3D& operator=(const Model3D&) = delete;

			static uPtr<Model3D> createMeshFromFile(
				EngineDevice& dvc,
				const std::string& filepath,
				Model3DLoadInfo loadInfo
			);
			static uPtr<Model3D> loadMeshFromFile(
				EngineDevice& dvc,
				const AssetID& asset,
				Model3DLoadInfo loadInfo
			);
			uint32_t bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer, uint32_t index);

			//		<Buffer Data>
			SubmeshBuffers buffers{};
			std::vector<std::string> materialSlots{};
			std::vector<AssetID> materials{};
		private:
			void createVertexBuffers(std::vector<SubmeshData*> mapdata);
			void createIndexBuffers(std::vector<SubmeshData*> mapdata);

			EngineDevice* device;
		};
	}
}
