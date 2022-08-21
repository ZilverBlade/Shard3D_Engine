#include "../../s3dpch.h" 
#include <glm/gtx/hash.hpp>

#include "model.h"

#include "../../core.h"
#include "../../ecs.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>           
#include <assimp/postprocess.h>     
#include <fstream>
#include "matmgr.h"

namespace std {
	template <>
	struct hash<Shard3D::EngineMesh::Vertex> {
		size_t operator()(Shard3D::EngineMesh::Vertex const& vertex) const {
			size_t seed = 0;
			Shard3D::hashCombine(seed, vertex.position, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Shard3D {

	EngineMesh::EngineMesh(EngineDevice& dvc, const EngineMesh::Builder &builder) : device(&dvc) {
		for (auto& submesh : builder.submeshes) {
			SubmeshBuffers _buffers{};
			auto& data = submesh.second;
			createVertexBuffers(data, _buffers);
			createIndexBuffers(data, _buffers);
			createMaterialBuffers(data, _buffers);
			buffers.push_back(_buffers);
		}
	}
	EngineMesh::~EngineMesh() {}

	uPtr<EngineMesh> EngineMesh::createMeshFromFile(EngineDevice& dvc, const std::string& filepath, MeshType modelType) {
		Builder builder{};	
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		
		std::ifstream f(filepath.c_str());
		if (!f.good()) { SHARD3D_ERROR("Invalid model, file '{0}' not found", filepath); return uPtr<EngineMesh>(nullptr); };

		builder.loadScene(filepath);
		
		if (ini.GetBoolValue("LOGGING", "log.MeshLoadInfo") == true) {
			uint64_t vcount{};
			for (auto& primitive : builder.submeshes)
				vcount += primitive.second.vertices.size();
			SHARD3D_INFO("Loaded model: '{0}'\n\t\tMesh vertex count: {1}", filepath, vcount);
		}
		return make_uPtr<EngineMesh>(dvc, builder);
	}

	void EngineMesh::createVertexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers) {
		vertexCount = static_cast<uint32_t>(submesh.vertices.size());
		SHARD3D_ASSERT(vertexCount >= 3 && "Vertex count must be at least 3");

		SHARD3D_INFO("vertices {0} {1}", submesh.material, vertexCount);

		VkDeviceSize bufferSize = sizeof(submesh.vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(submesh.vertices[0]);

		EngineBuffer stagingBuffer{
			*device,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)submesh.vertices.data());

		_buffers.vertexBuffer = make_sPtr<EngineBuffer>(
			*device,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		device->copyBuffer(stagingBuffer.getBuffer(), _buffers.vertexBuffer->getBuffer(), bufferSize);
	}

	void EngineMesh::createIndexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers) {
		indexCount = static_cast<uint32_t>(submesh.indices.size());
		SHARD3D_INFO("indexCount {0} {1}", submesh.material, indexCount);

		_buffers.hasIndexBuffer = indexCount > 0;

		if (!_buffers.hasIndexBuffer) return;
		VkDeviceSize bufferSize = sizeof(submesh.indices[0]) * indexCount;
		uint32_t indexSize = sizeof(submesh.indices[0]);

		EngineBuffer stagingBuffer{
			*device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)submesh.indices.data());
		
		_buffers.indexBuffer = make_sPtr<EngineBuffer>(
			*device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		device->copyBuffer(stagingBuffer.getBuffer(), _buffers.indexBuffer->getBuffer(), bufferSize);
	}

	void EngineMesh::createMaterialBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers) {
		_buffers.material = AssetManager::retrieveMaterial(submesh.material);
		_buffers.material->createMaterialShader(*device, SharedPools::staticMaterialPool);
	}

	void EngineMesh::bind(VkCommandBuffer commandBuffer, SubmeshBuffers buffers) {
		VkBuffer _buffers[] = { buffers.vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, _buffers, offsets);
		if (buffers.hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, buffers.indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}	
	}
	void EngineMesh::bindMaterial(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, SubmeshBuffers buffers) {
		SHARD3D_ASSERT(buffers.material != nullptr && "Cannot bind a mesh without a material!");
		buffers.material->bind(commandBuffer, globalDescriptorSet);	// Not as optimised, but can be improved with previous suggestions
	}
	void EngineMesh::draw(VkCommandBuffer commandBuffer, SubmeshBuffers buffers) {
		if (buffers.hasIndexBuffer) {
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
		attributeDescriptions.reserve(3); 
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)	});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)	});
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32_SFLOAT,	offsetof(Vertex, uv)		});
		return attributeDescriptions;
	}
	
	void EngineMesh::Builder::loadScene(const std::string& filepath) {
		auto newTime = std::chrono::high_resolution_clock::now();
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath,
			aiProcess_Triangulate |
			aiProcess_OptimizeMeshes |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenNormals		
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			SHARD3D_ERROR("Error importing file '{0}': {1}", filepath, importer.GetErrorString());
			return;
		}
		submeshes.clear();
		processNode(scene->mRootNode, scene);
		
		SHARD3D_LOG("Duration of loading {0}: {1} ms", filepath,
			std::chrono::duration<float, std::chrono::milliseconds::period>
			(std::chrono::high_resolution_clock::now() - newTime).count());
	}

	void EngineMesh::Builder::processNode(aiNode* node, const aiScene* scene) {
		// process all the node's meshes (if any)
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			loadSubmesh(mesh, scene);
		}
		// then do the same for each of its children
		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	// experiments

//#define CombineMaterials
//#define CustomIndexing

	void EngineMesh::Builder::loadSubmesh(aiMesh* mesh, const aiScene* scene) {
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		const std::string& materialSlot =
#ifndef CombineMaterials
			 material->GetName().C_Str();
#endif
#ifdef CombineMaterials
		"material";
#endif
		if (submeshes.find(materialSlot) == submeshes.end()) { // create new slot if none exist
			SubmeshData submesh{}; 
			submeshes[materialSlot] = submesh;
		}
		hashMap<Vertex, uint32_t> uniqueVertices{};
		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex{};
			vertex.position = {
				mesh->mVertices[i].x,
				mesh->mVertices[i].z,
				mesh->mVertices[i].y
			};
			vertex.normal = {
				mesh->mNormals[i].x,
				mesh->mNormals[i].z,
				mesh->mNormals[i].y
			};
			if (mesh->mTextureCoords[0])
				vertex.uv = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			else vertex.uv = { 0.f, 0.f };
#ifdef CustomIndexing
			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(submeshes[materialSlot].vertices.size());
#endif
				submeshes[materialSlot].vertices.push_back(vertex);
#ifdef CustomIndexing
			}
			submeshes[materialSlot].indices.push_back(uniqueVertices[vertex]);
#endif
		}

#ifndef CustomIndexing
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				submeshes[materialSlot].indices.push_back(face.mIndices[j]);
		}
#endif

		SHARD3D_INFO("vertices {0} {1}", materialSlot, submeshes[materialSlot].vertices.size());
		SHARD3D_INFO("indexCount {0} {1}", materialSlot, submeshes[materialSlot].indices.size());
		aiColor4D color{};
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);
		float specular{};
		aiGetMaterialFloat(material, AI_MATKEY_SPECULAR_FACTOR, &specular);
		float shininess{}; { float roughness{};
		aiGetMaterialFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, &roughness); shininess = 1.f - roughness; }
		float metallic{};
		aiGetMaterialFloat(material, AI_MATKEY_METALLIC_FACTOR, &metallic);

	// We just create a simple default grid texture for all meshes
		sPtr<SurfaceMaterial_ShadedOpaque> grid_material = make_sPtr<SurfaceMaterial_ShadedOpaque>();
		grid_material->materialTag = materialSlot;
		grid_material->diffuseColor = { 1.f, 1.f, 1.f };
		grid_material->diffuseTex = ENGINE_ERRMTX;
		grid_material->specular = 1.0f;
		grid_material->shininess = 0.5f;
		
		submeshes[materialSlot].material = std::string("assets/materialdata/" + materialSlot + ".wbmat");
		AssetManager::emplaceMaterial(grid_material, std::string("assets/materialdata/" + materialSlot + ".wbmat"));

		SHARD3D_WARN("Loaded material slot {0}", materialSlot);
	}
}
