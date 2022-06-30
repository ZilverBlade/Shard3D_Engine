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
			//inline static std::vector<std::unordered_map<LOD_Level, EngineModel>> modelAssets;
			//inline static std::unordered_map<uint64_t, EngineTexture> textureAssets;

			//inline static std::unordered_map<MaterialSystem::MaterialList, EngineModel> materialAssets;

			void loadLevelAssets(std::shared_ptr<Level>& level) {
				MaterialSystem::MaterialList meshMatLst;
				meshMatLst.list.push_back(MaterialSystem::Material());
				//materialAssets.emplace(meshMatLst, nullptr);
			}
		};
	}
}