#pragma once
#include <vector>
#include "../components.hpp"
#include "../systems/material_system.hpp"
#include "level.hpp"
#include "../texture.hpp"

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
			static void loadMaterialsFromList(MaterialSystem::MaterialList);
			static void emplaceMaterial(MaterialSystem::Material material);
			MaterialSystem::Material retrieveMaterialByGUID(uint64_t guid);
		private:
			inline static std::unordered_map<std::string, EngineTexture> textureAssets;
			inline static std::unordered_map<uint64_t, MaterialSystem::Material> materialAssets;
			inline static std::unordered_map<uint64_t, MaterialSystem::MaterialList> materialListAssets;
		};
	}
}