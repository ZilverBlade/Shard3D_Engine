#include "../../s3dpch.h" 
#include "../../core.h"

#include "assetmgr.h"
#include "matmgr.h"

#include <fstream>
#include <filesystem>

namespace Shard3D {
	static std::string& castToAssetString(const std::string& input) {
		std::string output = input.substr(input.rfind("assets\\" - 7));
		std::for_each(output.begin(), output.end(), [](char& c) {
			c = ::tolower(c);
		});
		std::replace(output.begin(), output.end(), '\\', '/');
		return output;
	}

	void AssetManager::clearAllAssetsAndDontAddDefaults() { textureAssets.clear(); meshAssets.clear(); /*materialAssets.clear(); */}
	void AssetManager::clearTextureAssets() {
		SHARD3D_INFO("Clearing all texture assets");
		vkDeviceWaitIdle(engineDevice->device());
		textureAssets.clear();
		emplaceTexture(ENGINE_ERRTEX, VK_FILTER_NEAREST);
		emplaceTexture(ENGINE_ERRMTX, VK_FILTER_NEAREST);
		emplaceTexture(ENGINE_WHTTEX, VK_FILTER_NEAREST);
		emplaceTexture(ENGINE_BLKTEX, VK_FILTER_NEAREST);
		emplaceTexture(ENGINE_NRMTEX, VK_FILTER_NEAREST);
	}	
	void AssetManager::clearMeshAssets() {
		SHARD3D_INFO("Clearing all mesh assets");
		vkDeviceWaitIdle(engineDevice->device());
		meshAssets.clear();
		emplaceMesh(ENGINE_ERRMSH);
		emplaceMesh(ENGINE_DEFAULT_MODEL_FILE);
	}
	void AssetManager::clearMaterialAssets() {
		SHARD3D_INFO("Clearing all material assets");
		vkDeviceWaitIdle(engineDevice->device());
		
		//sPtr<SurfaceMaterial_ShadedOpaque> defaultMaterial = make_sPtr<SurfaceMaterial_ShadedOpaque>();
		//
		//defaultMaterial->diffuseTex = ENGINE_ERRMAT;
		//defaultMaterial->shininess = 0.f;
		//defaultMaterial->specular = 0.1f;
		//
		//IOUtils::writeStackBinary(defaultMaterial.get(), sizeof(SurfaceMaterial_ShadedOpaque), "assets/test.bin");
//		//defaultMaterial->createMaterialShader(SharedPools::staticMaterialPool.get());

		//surfaceMaterialAssets[defaultMaterial->materialTag] = defaultMaterial;


		//MaterialManager::saveMaterial(defaultMaterial, "assets/_engine/mat/grid.wbasset");
	}
	void AssetManager::clearAllAssets() {
		clearTextureAssets();
		clearMeshAssets();
		clearMaterialAssets();
	}
	void AssetManager::loadLevelAssets() {
		for (const auto& file : std::filesystem::recursive_directory_iterator("assets/materialdata")) { 
			//auto material = MaterialSystem::loadMaterial(file.path().string());
			//materialAssets[1] = material;
		}
	}
#pragma region Mesh
	void AssetManager::emplaceMesh(const std::string& meshPath, MeshType meshType) {	
		//cast to lowercase, otherwise upper case paths (which are the same as lowercase) will get treated as diff
		std::string _meshPath = meshPath;

		for (const auto& i : meshAssets) 
			if (i.first.find(_meshPath) != std::string::npos && _meshPath.find(i.first) != std::string::npos) {
				SHARD3D_WARN("Mesh at path '{0}' already exists! Mesh will be ignored...", _meshPath);
				return;		
		/*	meshAssets.erase(i.first);
			meshAssets[mesh->fpath] = mesh;
			return; //prevent entries in unordered map with same path */			
			}

		sPtr<EngineMesh> mesh = EngineMesh::createMeshFromFile(*engineDevice, _meshPath, meshType);
		if (!mesh) return;
		SHARD3D_LOG("Loaded mesh to asset map '{0}'", _meshPath);
		meshAssets[_meshPath] = mesh;
	}
	sPtr<EngineMesh>& AssetManager::retrieveMesh_ENSET_CONFIDENT_ASSETS(const std::string& path) {
		return meshAssets.at(path);
	}
	sPtr<EngineMesh>& AssetManager::retrieveMesh_NENSET_CONFIDENT_ASSETS(const std::string& path) {
		if (meshAssets.find(path) != meshAssets.cend())
			return meshAssets.at(path);
		return meshAssets.at(ENGINE_ERRMSH);
	}
#pragma endregion
#pragma region Texture
	void AssetManager::emplaceTexture(const std::string& texturePath, int filter) {
		std::string _texturePath = texturePath;
		for (const auto& i : textureAssets)
			if (i.first.find(_texturePath) != std::string::npos && _texturePath.find(i.first) != std::string::npos) {
				SHARD3D_WARN("Texture at path '{0}' already exists! Texture will be ignored...", texturePath);
				return;
				/*	meshAssets.erase(i.first);
					meshAssets[mesh->fpath] = mesh;
					return; //prevent entries in unordered map with same path */
			}
		sPtr<EngineTexture> texture = EngineTexture::createTextureFromFile(*engineDevice, _texturePath, static_cast<VkFilter>(filter));
		if (!texture) return;
		SHARD3D_LOG("Loaded texture to asset map '{0}'", _texturePath);
		textureAssets[_texturePath] = texture;
	}

	
	sPtr<EngineTexture>& AssetManager::retrieveTexture_ENSET_CONFIDENT_ASSETS(const std::string& path) {
		return textureAssets.at(path);
	}
	sPtr<EngineTexture>& AssetManager::retrieveTexture_NENSET_CONFIDENT_ASSETS(const std::string& path) {
		if (textureAssets.find(path) != textureAssets.cend())
			return textureAssets.at(path);
		return textureAssets.at(ENGINE_ERRTEX);
	}
#pragma endregion

#pragma region Material
	void AssetManager::emplaceMaterial(sPtr<SurfaceMaterial> material, const std::string& materialPath) {
		surfaceMaterialAssets[materialPath] = material;
	}


	sPtr<SurfaceMaterial>& AssetManager::retrieveSurfaceMaterial_NENSET_CONFIDENT_ASSETS(const std::string& path) {
		//if (surfaceMaterialAssets.find(path) != surfaceMaterialAssets.cend())
			return surfaceMaterialAssets.at(path);
		//return surfaceMaterialAssets.at(ENGINE_ERRMAT);
	}
#pragma endregion
	
}

#pragma region special
void Shard3D::_special_assets::_editor_icons_load() {
	for (auto icon : _editor_icons_array) {
		auto& readIco = icon[1];
		sPtr<EngineTexture> tex = 
			EngineTexture::createTextureFromFile(*AssetManager::engineDevice, icon[1],
				(readIco == "assets/_engine/tex/_editor/icon_null") ? VK_FILTER_NEAREST : VK_FILTER_LINEAR);
		_editor_icons[icon[0]] = tex;
	}
}

void Shard3D::_special_assets::_editor_icons_destroy() {
	_editor_icons.clear();
}
#pragma endregion