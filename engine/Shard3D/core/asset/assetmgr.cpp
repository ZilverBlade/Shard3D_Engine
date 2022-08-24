#include "../../s3dpch.h" 
#include "../../core.h"

#include "assetmgr.h"
#include "matmgr.h"

#include <fstream>
#include <filesystem>

namespace Shard3D {
	std::string AssetUtils::truncatePath(const std::string& total) {
		return total.substr(total.find_last_of("\\"));
	}

	AssetType AssetUtils::discoverAssetType(const std::string& assetPath) {
		if (strUtils::hasEnding(assetPath, ".s3dlevel")) return AssetType::Level;
		return AssetType::Unknown;
		std::ifstream input{ assetPath };
		input.open(assetPath);
		std::stringstream stream; stream << input.rdbuf();
		std::string file = stream.str();
		{
			std::string itemType = file.substr(sizeof("<asset_type>"), file.find("<asset_file>") - 2 - sizeof("<asset_type>"));
			if (itemType == "mesh3d") {
				input.close();
				return AssetType::Mesh3D;
			}
			if (itemType == "texture") {
				input.close();
				return AssetType::Texture;
			}
			if (itemType == "mesh3d") {
				input.close();
				return AssetType::Mesh3D;
			}
			if (itemType == "material") {
				input.close();
				return AssetType::Material;
			}
		}
		return AssetType::Unknown;
	}

	void ResourceHandler::destroy() {
		textureAssets.clear(); 
		meshAssets.clear(); 
		surfaceMaterialAssets.clear(); 
	}
	void ResourceHandler::clearTextureAssets() {
		SHARD3D_INFO("Clearing all texture assets");
		vkDeviceWaitIdle(engineDevice->device());
		textureAssets.clear();
	    loadTexture(AssetID(ENGINE_ERRTEX ENGINE_ASSET_SUFFIX));
	    loadTexture(AssetID(ENGINE_ERRMTX ENGINE_ASSET_SUFFIX));
	    loadTexture(AssetID(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX));
	    loadTexture(AssetID(ENGINE_BLKTEX ENGINE_ASSET_SUFFIX));
	    loadTexture(AssetID(ENGINE_NRMTEX ENGINE_ASSET_SUFFIX));
	}
	void ResourceHandler::clearMeshAssets() {
		SHARD3D_INFO("Clearing all mesh assets");
		vkDeviceWaitIdle(engineDevice->device());
		meshAssets.clear();
		loadMesh(AssetID(ENGINE_ERRMSH ENGINE_ASSET_SUFFIX));
		loadMesh(AssetID(ENGINE_DEFAULT_MODEL_FILE ENGINE_ASSET_SUFFIX));
	}
	void ResourceHandler::clearMaterialAssets() {
		SHARD3D_INFO("Clearing all material assets");
		vkDeviceWaitIdle(engineDevice->device());

		rPtr<SurfaceMaterial_ShadedOpaque> grid_material = make_rPtr<SurfaceMaterial_ShadedOpaque>();
		grid_material->materialTag = "world_grid";
		grid_material->diffuseColor = { 1.f, 1.f, 1.f };
		grid_material->diffuseTex = AssetID(ENGINE_ERRMTX ENGINE_ASSET_SUFFIX);
		grid_material->specular = 0.5f;
		grid_material->shininess = 0.2f;

		loadMaterial(grid_material, "world_grid");
	}
	void ResourceHandler::clearAllAssets() {
		clearTextureAssets();
		clearMeshAssets();
		clearMaterialAssets();
	}

//========================================================================================================================

	void AssetManager::importTexture(const std::string& sourcepath, const std::string& destpath, TextureLoadInfo info) {
		if (!(strUtils::hasStarting(destpath, "assets/") || strUtils::hasStarting(destpath, "assets\\"))) {
			SHARD3D_ERROR("Cannot emplace a texture outside of the assets folder!");
			return;
		}
		std::ifstream ifile(destpath);
		if (!ifile.good()) std::filesystem::copy_file(sourcepath, destpath);

		std::ofstream out(destpath +  ENGINE_ASSET_SUFFIX);
		out << "<asset_type>:texture\n";
		out << "<asset_file>:" << destpath << "\n";
		out << "<asset_orig>:" << sourcepath << "\n";
						   
		out << "<properties>:\n";

		TextureLoadInfo _info[1] = { info };
		std::vector<uint8_t> data = IOUtils::getStackBinary(_info, sizeof(TextureLoadInfo));
		for (auto& byte : data) out << byte;

		out.flush();
		out.close();
	}

	void AssetManager::importMesh(const std::string& sourcepath, const std::string& destpath, MeshLoadInfo info) {
		if (!(strUtils::hasStarting(destpath, "assets/") || strUtils::hasStarting(destpath, "assets\\"))) {
			SHARD3D_ERROR("Cannot emplace a texture outside of the assets folder!");
			return;
		}
		std::ifstream ifile(destpath);
		if (!ifile.good()) std::filesystem::copy_file(sourcepath, destpath);

		std::ofstream out(destpath +  ENGINE_ASSET_SUFFIX);
		out << "<asset_type>:mesh3d\n";
		out << "<asset_file>:" << destpath << "\n";
		out << "<asset_orig>:" << sourcepath << "\n";

		out << "<properties>:\n";

		MeshLoadInfo _info[1] = { info };
		std::vector<uint8_t> data = IOUtils::getStackBinary(_info, sizeof(MeshLoadInfo));
		for (auto& byte : data) out << byte;

		out.flush();
		out.close();
	}

	void AssetManager::createMaterial(const std::string& destpath, rPtr<SurfaceMaterial> material) {

	}

#pragma region Mesh

	void ResourceHandler::_loadMesh(const AssetID& assetPath) {
		std::fstream input{ assetPath.getFile(), std::ios::binary | std::ios::in };
		std::streampos fileSize;
		input.unsetf(std::ios::skipws);

		input.seekg(0, std::ios::end);
		fileSize = input.tellg();
		input.seekg(0, std::ios::beg);

		std::string bytes;
		bytes.reserve(fileSize);

		bytes.insert(bytes.begin(),
			std::istream_iterator<uint8_t>(input),
			std::istream_iterator<uint8_t>());

		{
			std::string itemType = bytes.substr(sizeof("<asset_type>"), bytes.find("<asset_file>") - 2 - sizeof("<asset_type>"));
			if (itemType != "mesh3d") {
				SHARD3D_ERROR("Trying to load non mesh asset!");
				return;
			}
		}
		// Hacky binary read
		MeshLoadInfo loadInfo = *reinterpret_cast<MeshLoadInfo*>(reinterpret_cast<uintptr_t>(bytes.substr(bytes.find("<properties>") + sizeof("<properties>  ")).data()));

		// Hacky entry reading
		size_t entryBegin = bytes.find("<asset_file>") + sizeof("<asset_file>");
		std::string meshFile = bytes.substr(entryBegin, bytes.find("<asset_orig>") - 2 - entryBegin);
		AssetID meshAsset = assetPath;
		input.close();
		if (meshAssets.find(meshAsset) != meshAssets.end()) {
			SHARD3D_WARN("Mesh at path '{0}' (ID {1}) already exists! Mesh will be ignored...", meshAsset.getFile(), meshAsset.getID());
			return;
		}

		rPtr<EngineMesh> mesh = EngineMesh::loadMeshFromFile(*engineDevice, meshFile, loadInfo);
		if (!mesh) return;
		SHARD3D_LOG("Loaded asset to resource map '{0}'", meshAsset.getFile());
		meshAssets[meshAsset] = mesh;
	}
	void ResourceHandler::loadMesh(const AssetID& asset) {
		reloadMeshQueue.push_back(asset);
	}
	rPtr<EngineMesh>& ResourceHandler::retrieveMesh_unsafe(const AssetID& asset) {
		return meshAssets.at(asset);
	}
	rPtr<EngineMesh>& ResourceHandler::retrieveMesh_safe(const AssetID& asset) {
		if (meshAssets.find(asset) != meshAssets.cend())
			return meshAssets.at(asset);
		return meshAssets.at(AssetID(ENGINE_ERRMSH ENGINE_ASSET_SUFFIX));
	}
#pragma endregion
#pragma region Texture
	void ResourceHandler::loadTexture(const AssetID& assetPath) {
		std::fstream input{ assetPath.getFile(), std::ios::binary | std::ios::in };
		std::streampos fileSize;
		input.unsetf(std::ios::skipws);

		input.seekg(0, std::ios::end);
		fileSize = input.tellg();
		input.seekg(0, std::ios::beg);

		std::string bytes; 
		bytes.reserve(fileSize);
		
		bytes.insert(bytes.begin(),
			std::istream_iterator<uint8_t>(input),
			std::istream_iterator<uint8_t>());

		{
			std::string itemType = bytes.substr(sizeof("<asset_type>"), bytes.find("<asset_file>") - 2 - sizeof("<asset_type>"));
			if (itemType != "texture") {
				SHARD3D_ERROR("Trying to load non texture asset!");
				return;
			}
		}
		// Hacky binary read
		TextureLoadInfo loadInfo = *reinterpret_cast<TextureLoadInfo*>(reinterpret_cast<uintptr_t>(bytes.substr(bytes.find("<properties>") + sizeof("<properties>  ")).data()));

		// Hacky entry reading
		size_t entryBegin = bytes.find("<asset_file>") + sizeof("<asset_file>");
		std::string textureFile = bytes.substr(entryBegin, bytes.find("<asset_orig>") - 2 - entryBegin);
		AssetID textureAsset = assetPath;
		input.close();
		if (textureAssets.find(textureAsset) != textureAssets.end()) {
			SHARD3D_WARN("Texture at path '{0}' (ID {1}) already exists! Texture will be ignored...", textureAsset.getFile(), textureAsset.getID());
			return;
		}
		rPtr<EngineTexture> texture = EngineTexture::createTextureFromFile(*engineDevice, textureFile, loadInfo);
		if (!texture) return;
		SHARD3D_LOG("Loaded texture to resource map '{0}'", textureAsset.getFile());
		textureAssets[textureAsset] = texture;
	}


	void ResourceHandler::runGarbageCollector() {
		if (reloadMeshQueue.size() != 0) {
			vkDeviceWaitIdle(engineDevice->device());
			for (AssetID& file : reloadMeshQueue) {
				_loadMesh(file);
			}
			reloadMeshQueue.clear();
			return;
		}	
	}

	rPtr<EngineTexture>& ResourceHandler::retrieveTexture_unsafe(const AssetID& asset) {
		return textureAssets.at(asset);
	}
	rPtr<EngineTexture>& ResourceHandler::retrieveTexture_safe(const AssetID& asset) {
		if (textureAssets.find(asset) != textureAssets.cend())
			return textureAssets.at(asset);
		return textureAssets.at(AssetID(ENGINE_ERRTEX ENGINE_ASSET_SUFFIX));
	}
#pragma endregion

#pragma region Material
	void ResourceHandler::loadMaterial(rPtr<SurfaceMaterial> material, const std::string& materialPath) {
		surfaceMaterialAssets[AssetID(materialPath)] = material;
		material->createMaterialShader(*engineDevice, SharedPools::staticMaterialPool);
	}

	rPtr<SurfaceMaterial>& ResourceHandler::retrieveSurfaceMaterial_safe(const AssetID& asset) {
		if (surfaceMaterialAssets.find(asset) != surfaceMaterialAssets.cend())
			return surfaceMaterialAssets.at(asset); return surfaceMaterialAssets.at(asset);
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
		rPtr<EngineTexture> tex =
			EngineTexture::createTextureFromFile(*AssetManager::engineDevice, icon[1],
				loadInfo);
		_editor_icons[icon[0]] = tex;
	}
}

void Shard3D::_special_assets::_editor_icons_destroy() {
	_editor_icons.clear();
}
#pragma endregion