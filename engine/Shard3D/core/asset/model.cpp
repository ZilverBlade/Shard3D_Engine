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
			buffers.push_back(_buffers);
			materialSlots.push_back(submesh.first);
			materials.push_back(data.materialAsset);
		}
	}
	EngineMesh::~EngineMesh() {}

	uPtr<EngineMesh> EngineMesh::createMeshFromFile(EngineDevice& dvc, const std::string& filepath, MeshLoadInfo loadInfo) {
		Builder builder{};	
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		
		std::ifstream f(filepath.c_str());
		if (!f.good()) { SHARD3D_ERROR("Invalid model, file '{0}' not found", filepath); return uPtr<EngineMesh>(nullptr); };

		builder.loadScene(filepath, true);
		
		if (ini.GetBoolValue("LOGGING", "log.MeshLoadInfo") == true) {
			uint64_t vcount{};
			for (auto& primitive : builder.submeshes)
				vcount += primitive.second.vertices.size();
			SHARD3D_INFO("Loaded model: '{0}'\n\t\tMesh vertex count: {1}", filepath, vcount);
		}
		return make_uPtr<EngineMesh>(dvc, builder);
	}

	uPtr<EngineMesh> EngineMesh::loadMeshFromFile(EngineDevice& dvc, const AssetID& asset, MeshLoadInfo loadInfo) {
		Builder builder{};
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		std::ifstream f(asset.getFile());
		if (!f.good()) { SHARD3D_ERROR("Invalid model, file '{0}' not found", asset.getFile()); return uPtr<EngineMesh>(nullptr); };

		builder.loadScene(asset, false);
		return make_uPtr<EngineMesh>(dvc, builder);
	}

	void EngineMesh::createVertexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers) {
		_buffers.vertexCount = static_cast<uint32_t>(submesh.vertices.size());
		SHARD3D_ASSERT(_buffers.vertexCount >= 3 && "Vertex count must be at least 3");

		VkDeviceSize bufferSize = sizeof(submesh.vertices[0]) * _buffers.vertexCount;
		uint32_t vertexSize = sizeof(submesh.vertices[0]);

		EngineBuffer stagingBuffer{
			*device,
			vertexSize,
			_buffers.vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)submesh.vertices.data());

		_buffers.vertexBuffer = make_sPtr<EngineBuffer>(
			*device,
			vertexSize,
			_buffers.vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		device->copyBuffer(stagingBuffer.getBuffer(), _buffers.vertexBuffer->getBuffer(), bufferSize);
	}

	void EngineMesh::createIndexBuffers(const SubmeshData& submesh, SubmeshBuffers& _buffers) {
		_buffers.indexCount = static_cast<uint32_t>(submesh.indices.size());
		_buffers.hasIndexBuffer = _buffers.indexCount > 0;

		if (!_buffers.hasIndexBuffer) return;
		VkDeviceSize bufferSize = sizeof(submesh.indices[0]) * _buffers.indexCount;
		uint32_t indexSize = sizeof(submesh.indices[0]);

		EngineBuffer stagingBuffer{
			*device,
			indexSize,
			_buffers.indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)submesh.indices.data());
		
		_buffers.indexBuffer = make_sPtr<EngineBuffer>(
			*device,
			indexSize,
			_buffers.indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		device->copyBuffer(stagingBuffer.getBuffer(), _buffers.indexBuffer->getBuffer(), bufferSize);
	}

	void EngineMesh::bind(VkCommandBuffer commandBuffer, SubmeshBuffers buffers) {
		VkBuffer _buffers[] = { buffers.vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, _buffers, offsets);
		if (buffers.hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, buffers.indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}	
	}

	void EngineMesh::draw(VkCommandBuffer commandBuffer, SubmeshBuffers buffers) {
		if (buffers.hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, buffers.indexCount, 1, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, buffers.vertexCount, 1, 0, 0);
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
	
	void EngineMesh::Builder::loadScene(const std::string& filepath, bool createMaterials) {
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

		workingDir = filepath.substr(0, filepath.find_last_of("/"));
		SHARD3D_LOG("{0}",workingDir);
		processNode(scene->mRootNode, scene, createMaterials);
		
		SHARD3D_LOG("Duration of loading {0}: {1} ms", filepath,
			std::chrono::duration<float, std::chrono::milliseconds::period>
			(std::chrono::high_resolution_clock::now() - newTime).count());
	}

	void EngineMesh::Builder::processNode(aiNode* node, const aiScene* scene, bool createMaterials) {
		// process all the node's meshes (if any)
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			loadSubmesh(mesh, scene, createMaterials);
		}
		// then do the same for each of its children
		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene, createMaterials);
		}
	}

	// experiments

//#define CombineMaterials
//#define CustomIndexing

	void EngineMesh::Builder::loadSubmesh(aiMesh* mesh, const aiScene* scene, bool createMaterials) {
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

		if (createMaterials) {
			aiColor4D color{1.f, 1.f, 1.f, 1.f};
			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);
			float specular{0.5f};
			aiGetMaterialFloat(material, AI_MATKEY_SPECULAR_FACTOR, &specular);
			float shininess{0.5f}; { float roughness{};
			aiGetMaterialFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, &roughness); shininess = 1.f - roughness; }
			float metallic{0.f};
			aiGetMaterialFloat(material, AI_MATKEY_METALLIC_FACTOR, &metallic);

			// We just create a simple default grid texture for all meshes
			rPtr<SurfaceMaterial_ShadedOpaque> grid_material = make_rPtr<SurfaceMaterial_ShadedOpaque>();
			grid_material->materialTag = materialSlot;
			grid_material->diffuseColor = { color.r, color.g, color.b };
			grid_material->specular = specular;
			grid_material->shininess = shininess;
			grid_material->metallic = metallic;
			grid_material->drawData.culling = VK_CULL_MODE_FRONT_BIT;

			AssetManager::createMaterial("assets/materialdata/" + materialSlot + ".s3dasset", grid_material);
		}
		AssetID m_asset = AssetID(ENGINE_ERRMAT ENGINE_ASSET_SUFFIX);
		if (AssetManager::doesAssetExist("assets/materialdata/" + materialSlot + ".s3dasset")) {
			m_asset = AssetID("assets/materialdata/" + materialSlot + ".s3dasset");
			if (!createMaterials) ResourceHandler::loadSurfaceMaterialRecursive(m_asset);
		}
		


		submeshes[materialSlot].materialAsset = m_asset;
	}
}
