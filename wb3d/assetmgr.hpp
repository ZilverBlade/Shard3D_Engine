#pragma once
#include <vector>
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
			/* Clears all of the asset maps
*/
			static void clearLevelAssets();
			/* Loads all of the materials in use by the level into the asset maps.
			Make sure to clear before loading, since you dont want to waste resources pointing to unused assets!
			*/
			static void loadLevelAssets();

			//unlike other assets, textures and models should use a path as a key, 
			//since they directly load from there, rather than load into a wrapper/struct

			static std::unordered_map<std::string, std::shared_ptr<EngineModel>>& getModelAssets() { return modelAssets; }
			static std::unordered_map<std::string, EngineTexture>& getTextureAssets() { return textureAssets; }

			static void emplaceModel(const std::shared_ptr<EngineModel>& model);
			static std::shared_ptr<EngineModel>& retrieveModel(const std::string& path);

			static void emplaceTexture(EngineTexture& model);
			static EngineTexture& retrieveTexture(const std::string& path);

#pragma region Material shenanigans
			static void loadMaterialsFromList(MaterialSystem::MaterialList& matlist);
			static void emplaceMaterial(MaterialSystem::Material& material);
			static MaterialSystem::Material retrieveMaterialByGUID(uint64_t guid);
			static MaterialSystem::Material retrieveMaterialByPath(const std::string& path);
			static std::unordered_map<uint64_t, MaterialSystem::Material>& getMaterialAssets() { return materialAssets; }
			static std::unordered_map<uint64_t, MaterialSystem::MaterialList>& getMaterialListAssets() { return materialListAssets; }
#pragma endregion	
		private:
			inline static std::unordered_map<std::string, std::shared_ptr<EngineModel>> modelAssets;
			inline static std::unordered_map<std::string, EngineTexture> textureAssets;
			inline static std::unordered_map<uint64_t, MaterialSystem::Material> materialAssets;
			inline static std::unordered_map<uint64_t, MaterialSystem::MaterialList> materialListAssets;
		};
	}
}