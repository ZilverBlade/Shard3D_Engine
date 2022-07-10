#include "../s3dtpch.h" 
#include "assetmgr.hpp"

#include "../singleton.hpp"
namespace Shard3D::wb3d {
	void AssetManager::clearAllAssetsAndDontAddDefaults() { textureAssets.clear(); meshAssets.clear(); materialAssets.clear(); }
	void AssetManager::clearTextureAssets() {
		SHARD3D_WARN("Clearing all texture assets, may cause unexpected crash!");
		vkDeviceWaitIdle(Singleton::engineDevice.device());
		textureAssets.clear();
		emplaceTexture(ENGINE_ERRTEX, VK_FILTER_NEAREST);
		emplaceTexture(ENGINE_ERRMAT, VK_FILTER_NEAREST);
	}	
	void AssetManager::clearMeshAssets() {
		SHARD3D_WARN("Clearing all mesh assets, may cause unexpected crash!");
		vkDeviceWaitIdle(Singleton::engineDevice.device());
		meshAssets.clear();
		emplaceMesh(ENGINE_ERRMSH);
		emplaceMesh(ENGINE_DEFAULT_MODEL_FILE);
	}
	void AssetManager::clearAllAssets() {
		clearTextureAssets();
		clearMeshAssets();
		materialAssets.clear();
	}
	void AssetManager::loadLevelAssets() {
		for (const auto& file : std::filesystem::recursive_directory_iterator("assets/materialdata")) { 
			auto material = MaterialSystem::loadMaterial(file.path().string());
			materialAssets[material.guid] = material;
		}
	}
#pragma region Mesh
	void AssetManager::emplaceMesh(const std::string& meshPath, MeshType meshType) {	
		//cast to lowercase, otherwise upper case paths (which are the same as lowercase) will get treated as diff
		for (const auto& i : meshAssets) 
			if (i.first.find(meshPath) != std::string::npos && meshPath.find(i.first) != std::string::npos) {
				SHARD3D_WARN("Mesh at path '{0}' already exists! Mesh will be ignored...", meshPath);
				return;		
		/*	meshAssets.erase(i.first);
			meshAssets[mesh->fpath] = mesh;
			return; //prevent entries in unordered map with same path */			
			}

		std::shared_ptr<EngineMesh> mesh = EngineMesh::createMeshFromFile(meshPath, meshType);
		if (!mesh) return;
		SHARD3D_LOG("Loaded mesh to asset map '{0}'", meshPath);
		meshAssets[meshPath] = mesh;
	}
	std::shared_ptr<EngineMesh>& AssetManager::retrieveMesh(const std::string& path) {
#if !ENSET_CONFIDENT_ASSETS
			if (meshAssets.find(path) != meshAssets.cend())
				return meshAssets.at(path);
			return meshAssets.at(ENGINE_ERRMSH);
#elif
		return meshAssets.at(path);
#endif
	}
#pragma endregion
#pragma region Texture
	void AssetManager::emplaceTexture(const std::string& texturePath, VkFilter filter) {
		//cast to lowercase, otherwise upper case paths (which are the same as lowercase) will get treated as diff
		for (const auto& i : textureAssets)
			if (i.first.find(texturePath) != std::string::npos && texturePath.find(i.first) != std::string::npos) {
				SHARD3D_WARN("Texture at path '{0}' already exists! Texture will be ignored...", texturePath);
				return;
				/*	meshAssets.erase(i.first);
					meshAssets[mesh->fpath] = mesh;
					return; //prevent entries in unordered map with same path */
			}
		std::shared_ptr<EngineTexture> texture = EngineTexture::createTextureFromFile(Singleton::engineDevice, texturePath, filter);
		if (!texture) return;
		SHARD3D_LOG("Loaded texture to asset map '{0}'", texturePath);
		textureAssets[texturePath] = texture;
	}
	std::shared_ptr<EngineTexture>& AssetManager::retrieveTexture(const std::string& path) {
#if !ENSET_CONFIDENT_ASSETS
		if (textureAssets.find(path) != textureAssets.cend())
			return textureAssets.at(path);
		return textureAssets.at(ENGINE_ERRTEX);
#elif
		return textureAssets.at(path);
#endif
	}
#pragma endregion
	void AssetManager::loadMaterialsFromList(MaterialSystem::MaterialList& matlist) {
		for (MaterialSystem::Material material : matlist.list) {
			materialAssets.emplace(material.guid, material);
		}
	}
	void AssetManager::emplaceMaterial(MaterialSystem::Material& material) {
		for (const auto& i : materialAssets)
			if (i.second.path == material.path) { 
				SHARD3D_WARN("Material at path '{0}' already exists! Material will be overwritten.", material.path); 
				materialAssets.erase(i.first);
				materialAssets[material.guid] = material;
				return; //prevent entries in unordered map with same path
			}
		SHARD3D_LOG("Saved material '{0}'", material.path);
		materialAssets[material.guid] = material;
	}

	MaterialSystem::Material AssetManager::retrieveMaterialByGUID(uint64_t guid) {
		return materialAssets.at(guid);
	}
	MaterialSystem::Material AssetManager::retrieveMaterialByPath(const std::string& path) {
		for (const auto& i : materialAssets)
			if (i.second.path == path)			
				return i.second;
		SHARD3D_ERROR("Couldn't find material from path '{0}'.", path);
	}
}

#pragma region special
void Shard3D::_special_assets::_editor_icons_load() {
	for (auto icon : _editor_icons_array) {
		std::shared_ptr<EngineTexture> tex = EngineTexture::createTextureFromFile(Singleton::engineDevice, icon[1], VK_FILTER_LINEAR);
		_editor_icons[icon[0]] = tex;
	}
}

void Shard3D::_special_assets::_editor_icons_destroy()
{
}
#pragma endregion