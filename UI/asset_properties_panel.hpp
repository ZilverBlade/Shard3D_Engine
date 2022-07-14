#pragma once
#include "../s3dtpch.h"
#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"

using namespace Shard3D::wb3d;
namespace Shard3D {
	class AssetPropertiesPanel {
	public:
		AssetPropertiesPanel(const std::string& asset);
		~AssetPropertiesPanel();

		void render();
	private:
		std::string currentAsset;
		bool isOpen;
		friend class AssetExplorerPanel;
	};
}