#pragma once

#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"
#include <filesystem>
#include "asset_properties_panel.hpp"
using namespace Shard3D::wb3d;
namespace Shard3D {
	class AssetExplorerPanel {
	public:
		AssetExplorerPanel();
		~AssetExplorerPanel();

		void render();
	private:
		VkDescriptorSet folderIcon;
		VkDescriptorSet fileIcon;

		std::filesystem::path currentDir;
		std::unordered_map<std::string, AssetPropertiesPanel> assetPropertiesRenderList;
	};
}