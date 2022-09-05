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
			void bind(VkCommandBuffer commandBuffer, SubmeshBuffers buffers);
			void draw(VkCommandBuffer commandBuffer, SubmeshBuffers buffers);

			//			<Buffer Data>
			std::vector<SubmeshBuffers> buffers{};
			std::vector<std::string> materialSlots{};
			std::vector<AssetID> materials{};
		private:
			void createVertexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);
			void createIndexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);

			EngineDevice* device;
		};
	}
}
