#pragma once
#include "../s3dtpch.h"
#include "../systems/material_system.hpp"
#include "level.hpp"
#include "../texture.hpp"
#include "../model.hpp"

namespace Shard3D {
	namespace wb3d {
		class AssetManager {
		protected:
			enum LOD_Level {
				HighDetail = 0,
				MediumDetal = 1,
				LowDetail = 2,
				Invisible = -1
			};
		public:
			//Clears textureAssets
			static void clearTextureAssets();
			//Clears meshAssets
			static void clearMeshAssets();
			//Clears all of the asset maps
			static void clearAllAssets();
			/* Loads all of the materials in use by the level into the asset maps.
			Make sure to clear before loading, since you dont want to waste resources pointing to unused assets!
			*/
			static void loadLevelAssets();

			//unlike other assets, textures and meshs should use a path as a key, 
			//since they directly load from there, rather than load into a wrapper/struct

			static std::unordered_map<std::string, std::shared_ptr<EngineMesh>>& getMeshAssets() { return meshAssets; }
			static std::unordered_map<std::string, std::shared_ptr<EngineTexture>>& getTextureAssets() { return textureAssets; }

			static void emplaceMesh(const std::string& meshPath, MeshType meshType = MeshType::MESH_TYPE_OBJ);
			static std::shared_ptr<EngineMesh>& retrieveMesh(const std::string& path);

			static void emplaceTexture(const std::string& texturePath, VkFilter filter = VK_FILTER_LINEAR);
			static std::shared_ptr<EngineTexture>& retrieveTexture(const std::string& path);

#pragma region Material shenanigans
			static void loadMaterialsFromList(MaterialSystem::MaterialList& matlist);
			static void emplaceMaterial(MaterialSystem::Material& material);
			static MaterialSystem::Material retrieveMaterialByGUID(uint64_t guid);
			static MaterialSystem::Material retrieveMaterialByPath(const std::string& path);
			static std::unordered_map<uint64_t, MaterialSystem::Material>& getMaterialAssets() { return materialAssets; }
			static std::unordered_map<uint64_t, MaterialSystem::MaterialList>& getMaterialListAssets() { return materialListAssets; }
#pragma endregion	
		private:
			inline static std::unordered_map<std::string, std::shared_ptr<EngineMesh>> meshAssets;
			inline static std::unordered_map<std::string, std::shared_ptr<EngineTexture>> textureAssets;
			inline static std::unordered_map<uint64_t, MaterialSystem::Material> materialAssets;
			inline static std::unordered_map<uint64_t, MaterialSystem::MaterialList> materialListAssets;
		};
	}
}