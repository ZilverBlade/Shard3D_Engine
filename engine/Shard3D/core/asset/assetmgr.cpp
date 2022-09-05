#include "../../s3dpch.h" 

#include "assetmgr.h"
#include "matmgr.h"

#include "../../core.h"
#include "../../utils/yaml_ext.h"

#include <fstream>
#include <filesystem>

namespace Shard3D {
	std::string AssetUtils::truncatePath(const std::string& total) {
		return total.substr(total.find_last_of("\\"));
	}

	AssetType AssetUtils::discoverAssetType(const std::string& assetPath) {
		if (strUtils::hasEnding(assetPath, ".s3dlevel")) return AssetType::Level;

		std::ifstream stream(assetPath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		stream.close();
		std::string itemType = data["AssetType"].as<std::string>();
		{
			if (itemType == "texture") 
				return AssetType::Texture;
			if (itemType == "mesh3d") 
				return AssetType::Model3D;
			if (itemType == "surface_material") 
				return AssetType::SurfaceMaterial;
		}
		return AssetType::Unknown;
	}

	bool AssetManager::doesAssetExist(const std::string& assetPath) {
		std::ifstream ifile(assetPath);
		return ifile.good();
	}

	void ResourceHandler::destroy() {
		textureAssets.clear(); 
		meshAssets.clear(); 
		surfaceMaterialAssets.clear(); 
	}

	void ResourceHandler::init(EngineDevice& dvc) { 
		if (engineDevice) {
			loadTexture(AssetID(ENGINE_ERRTEX ENGINE_ASSET_SUFFIX));
			loadTexture(AssetID(ENGINE_ERRMTX ENGINE_ASSET_SUFFIX));
			loadTexture(AssetID(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX));
			loadTexture(AssetID(ENGINE_BLKTEX ENGINE_ASSET_SUFFIX));
			loadTexture(AssetID(ENGINE_NRMTEX ENGINE_ASSET_SUFFIX));
			loadMesh(AssetID(ENGINE_ERRMSH ENGINE_ASSET_SUFFIX));
			loadMesh(AssetID(ENGINE_DEFAULT_MODEL_FILE ENGINE_ASSET_SUFFIX));
			loadSurfaceMaterial(AssetID(ENGINE_ERRMAT ENGINE_ASSET_SUFFIX));
		}
		engineDevice = &dvc; 
	}

	// Check if is a core texture
	static bool isCoreAsset_T(AssetKey asset) {
		return	asset == AssetID(ENGINE_ERRTEX ENGINE_ASSET_SUFFIX) ||
				asset == AssetID(ENGINE_ERRMTX ENGINE_ASSET_SUFFIX) ||
				asset == AssetID(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX) ||
				asset == AssetID(ENGINE_BLKTEX ENGINE_ASSET_SUFFIX) ||
				asset == AssetID(ENGINE_NRMTEX ENGINE_ASSET_SUFFIX);
	}

	// Check if is a core mesh
	static bool isCoreAsset_M(AssetKey asset) {
		return	asset == AssetID(ENGINE_ERRMSH ENGINE_ASSET_SUFFIX) ||
			asset == AssetID(ENGINE_DEFAULT_MODEL_FILE ENGINE_ASSET_SUFFIX);
	}

	// Check if is a core surface material
	static bool isCoreAsset_S(AssetKey asset) {
		return asset == AssetID(ENGINE_ERRMAT ENGINE_ASSET_SUFFIX);
	}

	void ResourceHandler::clearTextureAssets() {
		SHARD3D_INFO("Clearing all texture assets");
		for (auto& asset : textureAssets) 
			if (!isCoreAsset_T(asset.first))
				ResourceHandler::unloadTexture(asset.first);
	}
	void ResourceHandler::clearMeshAssets() {
		SHARD3D_INFO("Clearing all mesh assets");
		for (auto& asset : meshAssets)
			if (!isCoreAsset_M(asset.first))
				ResourceHandler::unloadMesh(asset.first);
	}
	void ResourceHandler::clearMaterialAssets() {
		SHARD3D_INFO("Clearing all material assets");
		for (auto& asset : surfaceMaterialAssets)
			if (!isCoreAsset_S(asset.first))
				ResourceHandler::unloadSurfaceMaterial(asset.first);
	}
	void ResourceHandler::clearAllAssets() {
		clearTextureAssets();
		clearMeshAssets();
		clearMaterialAssets();
		runGarbageCollector();
	}

//========================================================================================================================

	void AssetManager::importTexture(const std::string& sourcepath, const std::string& destpath, TextureLoadInfo info) {
		if (!(strUtils::hasStarting(destpath, "assets/") || strUtils::hasStarting(destpath, "assets\\"))) {
			SHARD3D_ERROR("Cannot emplace a texture outside of the assets folder!");
			return;
		}
		std::ifstream ifile(destpath);
		if (!ifile.good()) std::filesystem::copy_file(sourcepath, destpath);

		
		std::ofstream fout(destpath + ENGINE_ASSET_SUFFIX);

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
		out << YAML::Key << "AssetType" << YAML::Value << "texture";
		out << YAML::Key << "AssetFile" << YAML::Value << destpath;
		out << YAML::Key << "AssetOrig" << YAML::Value << sourcepath;
		TextureLoadInfo _info[1] = { info };
		std::vector<uint8_t> data = IOUtils::getStackBinary(_info, sizeof(TextureLoadInfo));

		out << YAML::Key << "Properties" << YAML::Value << YAML::Binary(data.data(), sizeof(TextureLoadInfo));
		out << YAML::EndMap;

		fout << out.c_str();

		fout.flush();
		fout.close();
	}

	void AssetManager::importMesh(const std::string& sourcepath, const std::string& destpath, Model3DLoadInfo info) {
		if (!(strUtils::hasStarting(destpath, "assets/") || strUtils::hasStarting(destpath, "assets\\"))) {
			SHARD3D_ERROR("Cannot emplace a texture outside of the assets folder!");
			return;
		}
		std::ifstream ifile(destpath);
		if (!ifile.good()) std::filesystem::copy_file(sourcepath, destpath);

		std::ofstream fout(destpath + ENGINE_ASSET_SUFFIX);

		uPtr<Model3D> tempreadInfo = Model3D::createMeshFromFile(*engineDevice, destpath, Model3DLoadInfo());

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
		out << YAML::Key << "AssetType" << YAML::Value << "mesh3d";
		out << YAML::Key << "AssetFile" << YAML::Value << destpath;
		out << YAML::Key << "AssetOrig" << YAML::Value << sourcepath;
		out << YAML::Key << "Materials" << YAML::Value << tempreadInfo->materials;
		
		Model3DLoadInfo _info[1] = { info };
		std::vector<uint8_t> data = IOUtils::getStackBinary(_info, sizeof(Model3DLoadInfo));

		out << YAML::Key << "Properties" << YAML::Value << YAML::Binary(data.data(), sizeof(Model3DLoadInfo));
		out << YAML::EndMap;

		fout << out.c_str();

		fout.flush();
		fout.close();
	}

	void AssetManager::createMaterial(const std::string& destpath, rPtr<SurfaceMaterial> material) {
		std::string& dest = const_cast<std::string&>(destpath);
		std::replace(dest.begin(), dest.end(), '\\', '/');

		MaterialManager::saveMaterial(material, dest, true);
	}

	void AssetManager::purgeAsset(const std::string& assetPath) {
		if (std::filesystem::remove(assetPath))
			std::cout << "file " << assetPath << " deleted.\n";
		else
			std::cout << "file " << assetPath << " not found.\n";
	}

	void ResourceHandler::runGarbageCollector() {
		if (destroyMeshQueue.size() != 0) {
			vkDeviceWaitIdle(engineDevice->device());
			for (AssetID& file : destroyMeshQueue) {
				meshAssets.erase(file);
			}
			destroyMeshQueue.clear();
		}
		if (destroyTexQueue.size() != 0) {
			vkDeviceWaitIdle(engineDevice->device());
			for (AssetID& file : destroyTexQueue) {
				textureAssets.erase(file);
			}
			destroyTexQueue.clear();
		}
		if (rebuildSurfaceMaterialQueue.size() != 0) {
			vkDeviceWaitIdle(engineDevice->device());
			for (rPtr<SurfaceMaterial>& material : rebuildSurfaceMaterialQueue) {
				_buildSurfaceMaterial(material);
			}
			rebuildSurfaceMaterialQueue.clear();
		}
		if (destroySurfaceMatQueue.size() != 0) {
			vkDeviceWaitIdle(engineDevice->device());
			for (AssetID& file : destroySurfaceMatQueue) {
				surfaceMaterialAssets.erase(file);
			}
			destroySurfaceMatQueue.clear();
		}
	}

#pragma region Mesh

	void ResourceHandler::loadMesh(const AssetID& assetPath) {
		std::fstream input{ assetPath.getFile(), std::ios::binary | std::ios::in };

		if (!input.good()) {
			SHARD3D_ERROR("Failed to load {0} (ID {1})", assetPath.getFile(), assetPath.getID());
			return;
		}

		std::ifstream stream(assetPath.getFile());
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		stream.close();
		if (data["AssetType"].as<std::string>() != "mesh3d") {
			SHARD3D_ERROR("Trying to load non texture asset!");
			return;
		} 
		
		// Hacky binary read
		Model3DLoadInfo loadInfo = *reinterpret_cast<Model3DLoadInfo*>(reinterpret_cast<uintptr_t>(data["Properties"].as<YAML::Binary>().data()));
		
		input.close();
		if (meshAssets.find(assetPath) != meshAssets.end()) 
			return;	
		
		rPtr<Model3D> mesh = Model3D::loadMeshFromFile(*engineDevice, data["AssetFile"].as<std::string>(), loadInfo);
		if (!mesh) return;
		SHARD3D_LOG("Loaded asset to resource map '{0}'", assetPath.getFile());
		mesh->materials = data["Materials"].as<std::vector<AssetID>>();
		for (auto& material : mesh->materials) ResourceHandler::loadSurfaceMaterialRecursive(material);
		meshAssets[assetPath] = mesh;
	}

	void ResourceHandler::unloadMesh(const AssetID& asset) {
		destroyMeshQueue.push_back(asset);
	}

	rPtr<Model3D>& ResourceHandler::retrieveMesh_unsafe(const AssetID& asset) {
		return meshAssets.at(asset);
	}
	rPtr<Model3D>& ResourceHandler::retrieveMesh_safe(const AssetID& asset) {
		if (meshAssets.find(asset) != meshAssets.cend())
			return meshAssets.at(asset);
		return meshAssets.at(AssetID(ENGINE_ERRMSH ENGINE_ASSET_SUFFIX));
	}
#pragma endregion

#pragma region Texture

	void ResourceHandler::loadTexture(const AssetID& assetPath) {
		std::fstream input{ assetPath.getFile(), std::ios::binary | std::ios::in };
		
		if (!input.good()) {
			SHARD3D_ERROR("Failed to load {0} (ID {1})", assetPath.getFile(), assetPath.getID());
			return;
		}

		std::ifstream stream(assetPath.getFile());
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		stream.close();
		if (data["AssetType"].as<std::string>() != "texture") {
			SHARD3D_ERROR("Trying to load non texture asset!");
			return;
		}

		// Hacky binary read
		TextureLoadInfo loadInfo = *reinterpret_cast<TextureLoadInfo*>(reinterpret_cast<uintptr_t>(data["Properties"].as<YAML::Binary>().data()));

		// Hacky entry reading
		AssetID textureAsset = assetPath;
		input.close();
		if (textureAssets.find(textureAsset) != textureAssets.end()) 
			return;
		
		rPtr<Texture2D> texture = Texture2D::createTextureFromFile(*engineDevice, data["AssetFile"].as<std::string>(), loadInfo);
		if (!texture) return;
		SHARD3D_LOG("Loaded texture to resource map '{0}'", textureAsset.getFile());
		textureAssets[textureAsset] = texture;
	}

	void ResourceHandler::unloadTexture(const AssetID& asset) {
		destroyTexQueue.push_back(asset);
	}

	rPtr<Texture2D>& ResourceHandler::retrieveTexture_unsafe(const AssetID& asset) {
		return textureAssets.at(asset);
	}
	rPtr<Texture2D>& ResourceHandler::retrieveTexture_safe(const AssetID& asset) {
		if (textureAssets.find(asset) != textureAssets.cend())
			return textureAssets.at(asset);
		return textureAssets.at(AssetID(ENGINE_ERRTEX ENGINE_ASSET_SUFFIX));
	}

#pragma endregion

#pragma region Material
	void ResourceHandler::loadSurfaceMaterial(const AssetID& asset) {
		if (surfaceMaterialAssets.find(asset) != surfaceMaterialAssets.cend()) return;
		rPtr<SurfaceMaterial> material = MaterialManager::loadSurfaceMaterial(asset);
		if (!material) return;
		surfaceMaterialAssets[asset] = material;
		rebuildSurfaceMaterial(material);
		SHARD3D_LOG("Loaded material to resource map '{0}'", asset.getFile());
	}

	void ResourceHandler::loadSurfaceMaterialRecursive(const AssetID& asset) {
		loadSurfaceMaterial(asset);
		surfaceMaterialAssets[asset]->loadAllTextures();
	}

	void ResourceHandler::unloadSurfaceMaterial(const AssetID& asset) {
		destroySurfaceMatQueue.push_back(asset);
	}

	void ResourceHandler::rebuildSurfaceMaterial(rPtr<SurfaceMaterial> material) {
		material->loadAllTextures();
		if (!material->isBuilt()) _buildSurfaceMaterial(material);	
		rebuildSurfaceMaterialQueue.push_back(material);
	}

	void ResourceHandler::_buildSurfaceMaterial(rPtr<SurfaceMaterial> material) {
		material->createMaterialShader(*engineDevice, SharedPools::staticMaterialPool);
	}

	rPtr<SurfaceMaterial>& ResourceHandler::retrieveSurfaceMaterial_safe(const AssetID& asset) {
		if (surfaceMaterialAssets.find(asset) != surfaceMaterialAssets.cend())
			return surfaceMaterialAssets.at(asset);
		return surfaceMaterialAssets.at(AssetID(ENGINE_ERRMAT ENGINE_ASSET_SUFFIX));
	}
#pragma endregion



}

#pragma region special
void Shard3D::_special_assets::_editor_icons_load() {
	for (auto icon : _editor_icons_array) {
		auto& readIco = icon[1];
		TextureLoadInfo loadInfo{};
		loadInfo.filter = (readIco == "assets/_engine/tex/_editor/icon_null") ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
		rPtr<Texture2D> tex =
			Texture2D::createTextureFromFile(*AssetManager::engineDevice, icon[1],
				loadInfo);
		_editor_icons[icon[0]] = tex;
	}
}

void Shard3D::_special_assets::_editor_icons_destroy() {
	_editor_icons.clear();
}
#pragma endregion