#include "assetmgr.hpp"
#include "../engine_logger.hpp"

namespace Shard3D::wb3d {
	void AssetManager::clearLevelAssets() {
		textureAssets.clear();
		materialAssets.clear();
	}
	void AssetManager::loadLevelAssets() {
		for (const auto& file : std::filesystem::recursive_directory_iterator("assets/materialdata")) { 
			auto material = MaterialSystem::loadMaterial(file.path().string());
			materialAssets[material.guid] = material;
		}
	}

	void AssetManager::emplaceModel(const std::shared_ptr<EngineModel>& model) {
// cant get this piece of shit to work lmao
		//  for (const auto& i : modelAssets) 
		//	if (i.second->fpath == model->fpath) {
		//		SHARD3D_WARN("Model at path '{0}' already exists! Model will be overwritten.", model->fpath);
		//		modelAssets.erase(i.first);
		//		modelAssets[model->fpath] = model;
		//		return; //prevent entries in unordered map with same path
		//	}
		//SHARD3D_LOG("Loaded model to asset map '{0}'", model->fpath);
		//modelAssets.try_emplace(model->fpath, &model);
	}
	std::shared_ptr<EngineModel>& AssetManager::retrieveModel(const std::string& path) {
		return modelAssets.at(path);
	}
	void AssetManager::emplaceTexture(EngineTexture& model) {

	}
	EngineTexture& AssetManager::retrieveTexture(const std::string& path) {
		return textureAssets.at(path);
	}
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