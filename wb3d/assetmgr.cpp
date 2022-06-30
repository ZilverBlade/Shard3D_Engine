#include "assetmgr.hpp"
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
	void AssetManager::emplaceMaterial(MaterialSystem::Material material) {
		materialAssets[material.guid] = material;
	}

	MaterialSystem::Material AssetManager::retrieveMaterialByGUID(uint64_t guid) {
		return materialAssets.at(guid);
	}
}