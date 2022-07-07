#include "../s3dtpch.h" 
#include "assetmgr.hpp"

#include "../singleton.hpp"
namespace Shard3D::wb3d {
	void AssetManager::clearLevelAssets() {
		SHARD3D_WARN("Clearing all stored assets, may cause unexpected crash!");
		vkDeviceWaitIdle(Globals::engineDevice.device());
		textureAssets.clear();
		materialAssets.clear();
		modelAssets.clear();
	}
	void AssetManager::loadLevelAssets() {
		for (const auto& file : std::filesystem::recursive_directory_iterator("assets/materialdata")) { 
			auto material = MaterialSystem::loadMaterial(file.path().string());
			materialAssets[material.guid] = material;
		}
	}
#pragma region Model
	void AssetManager::emplaceModel(const std::string& modelPath, ModelType modelType) {		
		for (const auto& i : modelAssets) 
			if (i.first.find(modelPath) != std::string::npos && modelPath.find(i.first) != std::string::npos) {
				SHARD3D_WARN("Model at path '{0}' already exists! Model will be ignored...", modelPath);
				return;		
			/*	modelAssets.erase(i.first);
				modelAssets[model->fpath] = model;
				return; //prevent entries in unordered map with same path */			
			}
		std::shared_ptr<EngineModel> model = EngineModel::createModelFromFile(modelPath, modelType);
		SHARD3D_LOG("Loaded model to asset map '{0}'", modelPath);
		modelAssets[modelPath] = model;
	}
	std::shared_ptr<EngineModel>& AssetManager::retrieveModel(const std::string& path) {
		return modelAssets.at(path);
	}
#pragma endregion
#pragma region Texture
	void AssetManager::emplaceTexture(const std::string& texturePath) {
		for (const auto& i : textureAssets)
			if (i.first.find(texturePath) != std::string::npos && texturePath.find(i.first) != std::string::npos) {
				SHARD3D_WARN("Model at path '{0}' already exists! Model will be ignored...", texturePath);
				return;
				/*	modelAssets.erase(i.first);
					modelAssets[model->fpath] = model;
					return; //prevent entries in unordered map with same path */
			}
		std::shared_ptr<EngineTexture> texture = EngineTexture::createTextureFromFile(Globals::engineDevice, texturePath);
		SHARD3D_LOG("Loaded texture to asset map '{0}'", texturePath);
		textureAssets[texturePath] = texture;
	}
	std::shared_ptr<EngineTexture>& AssetManager::retrieveTexture(const std::string& path) {
		return textureAssets.at(path);
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