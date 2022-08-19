#pragma once
#include "../../vulkan_abstr.h"
#include "material.h"

extern "C" {
	struct aiNode;
	struct aiScene;
	struct aiMesh;
}
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

			std::string material{}; // Material path
		};

		struct SubmeshBuffers {
			sPtr<EngineBuffer> indexBuffer{};
			sPtr<EngineBuffer> vertexBuffer{};
			bool hasIndexBuffer = false;

			sPtr<SurfaceMaterial> material;
		};

		struct Builder {
			//    <Material Slot> <Submesh>
			hashMap<std::string, SubmeshData> submeshes;
			void loadScene(const std::string& filepath);
			void processNode(aiNode* node, const aiScene* scene);
			void loadSubmesh(aiMesh* mesh, const aiScene* scene);
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

		void bind(VkCommandBuffer commandBuffer, SubmeshBuffers buffers);
		void bindMaterial(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, SubmeshBuffers buffers);
		void draw(VkCommandBuffer commandBuffer, SubmeshBuffers buffers);

		VkPipelineLayout getMaterialPipelineLayout(SubmeshBuffers buffers) { return buffers.material->getPipelineLayout(); }
		//			<Buffer Data>
		std::vector<SubmeshBuffers> buffers{};

	private:
		void createVertexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);
		void createIndexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);
		void createMaterialBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers);

		uint32_t vertexCount{};

		uint32_t indexCount{};
		EngineDevice* device;
	};
}
