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
	struct hash<Shard3D::Resources::Model3D::Vertex> {
		size_t operator()(Shard3D::Model3D::Vertex const& vertex) const {
			size_t seed = 0;
			Shard3D::hashCombine(seed, vertex.position, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Shard3D::Resources {

	static bool useHighPVBuffer(){
		static bool hasChecked = false;
		static bool highpAttributes = false;
		if (hasChecked) return highpAttributes;
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		highpAttributes = ini.GetBoolValue("RENDERING", "HighPrecisionVBuffer");
		hasChecked = true;
		return highpAttributes;
	}

	Model3D::Model3D(EngineDevice& dvc, const Model3D::Builder &builder) : device(&dvc) {
		createVertexBuffers(builder.submeshPtrs);
		createIndexBuffers(builder.submeshPtrs);
		for (int i = 0; i < builder.submeshes.size(); i++) {
			materialSlots.push_back(builder.slotPtrs[i]);
			materials.push_back(builder.submeshPtrs[i]->materialAsset);

			SHARD3D_LOG("Creating buffer for slot {0} with index {1}", builder.slotPtrs[i], i);
		}
	}
	Model3D::~Model3D() {}

	uPtr<Model3D> Model3D::createMeshFromFile(EngineDevice& dvc, const std::string& filepath, Model3DLoadInfo loadInfo) {
		Builder builder{};	
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		
		std::ifstream f(filepath.c_str());
		if (!f.good()) { SHARD3D_ERROR("Invalid model, file '{0}' not found", filepath); return uPtr<Model3D>(nullptr); };

		builder.loadScene(filepath, true);
		
		if (ini.GetBoolValue("LOGGING", "log.Model3DLoadInfo") == true) {
			uint64_t vcount{};
			for (auto& primitive : builder.submeshPtrs)
				vcount += primitive->vertices.size();
			SHARD3D_INFO("Loaded model: '{0}'\n\t\tMesh vertex count: {1}", filepath, vcount);
		}
		return make_uPtr<Model3D>(dvc, builder);
	}

	uPtr<Model3D> Model3D::loadMeshFromFile(EngineDevice& dvc, const AssetID& asset, Model3DLoadInfo loadInfo) {
		Builder builder{};
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		std::ifstream f(asset.getFile());
		if (!f.good()) { SHARD3D_ERROR("Invalid model, file '{0}' not found", asset.getFile()); return uPtr<Model3D>(nullptr); };

		builder.loadScene(asset, false);

		return make_uPtr<Model3D>(dvc, builder);
	}

	void Model3D::createVertexBuffers(std::vector<SubmeshData*> mapdata) {
		std::vector<Vertex> vertexData;
		std::vector<LPVertex> lp_vertexData;

		if (useHighPVBuffer())
			for (int i = 0; i < mapdata.size(); i++) {
				uint32_t vertexcount = static_cast<uint32_t>(mapdata[i]->vertices.size());
				SHARD3D_ASSERT(vertexcount >= 3 && "Vertex count must be at least 3");
				buffers.vertexCounts.emplace_back(vertexcount);
				vertexData.reserve(vertexcount);
				for (Vertex vertex : mapdata[i]->vertices)
					vertexData.emplace_back(vertex);
				if (buffers.vertexOffsets.size() == 0) { buffers.vertexOffsets.push_back(0); continue; }
				buffers.vertexOffsets.push_back(buffers.vertexOffsets[i - 1] + buffers.vertexCounts[i - 1]);
			}
		else
			for (int i = 0; i < mapdata.size(); i++) {
				uint32_t vertexcount = static_cast<uint32_t>(mapdata[i]->vertices.size());
				SHARD3D_ASSERT(vertexcount >= 3 && "Vertex count must be at least 3");
				buffers.vertexCounts.emplace_back(vertexcount);
				lp_vertexData.reserve(vertexcount);
				for (Vertex vertex : mapdata[i]->vertices)
					lp_vertexData.emplace_back(vertex);
				if (buffers.vertexOffsets.size() == 0) { buffers.vertexOffsets.push_back(0); continue; }
				buffers.vertexOffsets.push_back(buffers.vertexOffsets[i - 1] + buffers.vertexCounts[i - 1]);
			}
		uint32_t fullVertexCount{};
		for (uint32_t count : buffers.vertexCounts) {
			fullVertexCount += count;
		}
	
		uint32_t vertexSize = useHighPVBuffer() ? sizeof(Vertex) : sizeof(LPVertex);
		VkDeviceSize bufferSize = static_cast<size_t>(vertexSize) * fullVertexCount;
		
		EngineBuffer stagingBuffer{
			*device,
			vertexSize,
			fullVertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(useHighPVBuffer() ? (void*)vertexData.data() : (void*)lp_vertexData.data());
		buffers.vertexBuffer = make_sPtr<EngineBuffer>(
			*device,
			vertexSize,
			fullVertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		device->copyBuffer(stagingBuffer.getBuffer(), buffers.vertexBuffer->getBuffer(), bufferSize);
	}

	void Model3D::createIndexBuffers(std::vector<SubmeshData*>mapdata) {
		std::vector<uint32_t> indexData;
		for (int i = 0; i < mapdata.size(); i++) {
			uint32_t indexcount = static_cast<uint32_t>(mapdata[i]->indices.size());
			buffers.indexCounts.emplace_back(indexcount);
			for (uint32_t index : mapdata[i]->indices)
				indexData.emplace_back(index);
			if (buffers.indexOffsets.size() == 0) { buffers.indexOffsets.push_back(0); continue; }
			buffers.indexOffsets.push_back(buffers.indexOffsets[i - 1] + buffers.indexCounts[i - 1]);
		}

		uint32_t fullIndexCount{};
		for (uint32_t count : buffers.indexCounts) {
			fullIndexCount += count;
		}

		buffers.hasIndexBuffer = fullIndexCount > 0;

		if (!buffers.hasIndexBuffer) return;
		VkDeviceSize bufferSize = sizeof(uint32_t) * fullIndexCount;
		uint32_t indexSize = sizeof(uint32_t);

		EngineBuffer stagingBuffer{
			*device,
			indexSize,
			fullIndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indexData.data());
		
		buffers.indexBuffer = make_sPtr<EngineBuffer>(
			*device,
			indexSize,
			fullIndexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		device->copyBuffer(stagingBuffer.getBuffer(), buffers.indexBuffer->getBuffer(), bufferSize);
	}

	uint32_t Model3D::bind(VkCommandBuffer commandBuffer) {
		VkBuffer _buffers[] = { buffers.vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, _buffers, offsets);
		
		if (buffers.hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, buffers.indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
		return buffers.vertexOffsets.size();
	}

	void Model3D::draw(VkCommandBuffer commandBuffer, uint32_t index) {
		if (buffers.hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, buffers.indexCounts[index], 1, buffers.indexOffsets[index], buffers.vertexOffsets[index], 0);
		} else {
			vkCmdDraw(commandBuffer, buffers.vertexCounts[index], 1, buffers.vertexOffsets[index], 0);
		}
	}

	std::vector<VkVertexInputBindingDescription> Model3D::Vertex::getBindingDescriptions() {
		uint32_t size = useHighPVBuffer() ? sizeof(Vertex) : sizeof(LPVertex);
		return { {0, size, VK_VERTEX_INPUT_RATE_VERTEX} };
	}
	
	std::vector<VkVertexInputAttributeDescription> Model3D::Vertex::getAttributeDescriptions() {
		if (useHighPVBuffer()) SHARD3D_LOG("Using High Precision Vertex Buffer");
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.reserve(3); 
		attributeDescriptions.push_back({ 0, 0, useHighPVBuffer() ? VK_FORMAT_R32G32B32_SFLOAT : VK_FORMAT_R16G16B16_SFLOAT, offsetof(Vertex, position)	});
		attributeDescriptions.push_back({ 1, 0, useHighPVBuffer() ? VK_FORMAT_R32G32B32_SFLOAT : VK_FORMAT_R16G16B16_SFLOAT, offsetof(Vertex, normal)	});
		attributeDescriptions.push_back({ 2, 0, useHighPVBuffer() ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R16G16_SFLOAT,	offsetof(Vertex, uv)		});
		return attributeDescriptions;
	}
	
	void Model3D::Builder::loadScene(const std::string& filepath, bool createMaterials) {
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

	void Model3D::Builder::processNode(aiNode* node, const aiScene* scene, bool createMaterials) {
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

	void Model3D::Builder::loadSubmesh(aiMesh* mesh, const aiScene* scene, bool createMaterials) {
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		const std::string& materialSlot = material->GetName().C_Str();

		if (submeshes.find(materialSlot) == submeshes.end()) { // create new slot if none exist
			SubmeshData submesh{};
			submeshes[materialSlot] = submesh;
			submeshPtrs.push_back(&submeshes[materialSlot]);
			slotPtrs.push_back(materialSlot);

			SHARD3D_LOG("Creating primitive data for slot {0} with index {1}", materialSlot, slotPtrs.size() - 1);
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

			submeshes[materialSlot].vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				submeshes[materialSlot].indices.push_back(face.mIndices[j]);
		}

		std::string cleanString = strUtils::removeIllegalPathChars(materialSlot);
		if (createMaterials) {
			aiColor4D color{1.f, 1.f, 1.f, 1.f};
			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);
			float specular{0.5f};
			aiGetMaterialFloat(material, AI_MATKEY_SPECULAR_FACTOR, &specular);
			float shininess{0.5f}; { float roughness{};
			aiGetMaterialFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, &roughness); shininess = 1.f - roughness; }
			float metallic{0.f};
			aiGetMaterialFloat(material, AI_MATKEY_METALLIC_FACTOR, &metallic);

			rPtr<SurfaceMaterial_Shaded> grid_material = make_rPtr<SurfaceMaterial_Shaded>();
			grid_material->materialTag = materialSlot;
			grid_material->diffuseColor = { color.r, color.g, color.b };
			grid_material->specular = specular;
			grid_material->shininess = shininess;
			grid_material->metallic = metallic;
			grid_material->setCullMode(VK_CULL_MODE_FRONT_BIT);

			AssetManager::createMaterial(workingDir + "/" + cleanString + ".s3dasset", grid_material);
		}
		AssetID m_asset = ResourceHandler::coreAssets.s_errorMaterial;
		if (IOUtils::doesFileExist(workingDir + "/" + cleanString + ".s3dasset")) {
			m_asset = AssetID(workingDir + "/" + cleanString + ".s3dasset");
		}

		submeshes[materialSlot].materialAsset = m_asset;
	}
	Model3D::LPVertex::LPVertex(const Vertex& highp)
	{
		
		myposX	 = half::ToFloat16(highp.position.x);
		myposY	 = half::ToFloat16(highp.position.y);
		myposZ	 = half::ToFloat16(highp.position.z);
		mynormX	 = half::ToFloat16(highp.normal.x);
		mynormY	 = half::ToFloat16(highp.normal.y);
		mynormZ	 = half::ToFloat16(highp.normal.z);
		myuvX	 = half::ToFloat16(highp.uv.x);
		myuvY	 = half::ToFloat16(highp.uv.y);
	}
}
