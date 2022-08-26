#pragma once

#include <Shard3D/core/ecs/level.h>
#include <Shard3D/core/ecs/actor.h>
#include <filesystem>

using namespace Shard3D::ECS;
namespace Shard3D {
	class AssetExplorerPanel {
		struct AssetIteratorCombo {
			std::vector<std::filesystem::directory_entry> entries;
			std::vector<VkDescriptorSet*> icons;
			std::vector<AssetType> types;
		};
	public:
		AssetExplorerPanel();
		~AssetExplorerPanel();

		void render();
	private:
		VkDescriptorSet folderIcon;
		VkDescriptorSet fileIcon;
		VkDescriptorSet mesh3dIcon;
		VkDescriptorSet textureIcon; 
		VkDescriptorSet levelIcon;
		VkDescriptorSet smatIcon;

		void refreshIterator(std::filesystem::path newPath);
		std::filesystem::path currentDir;
		std::filesystem::path currentContextMenu;
		AssetIteratorCombo directoryEntries;
	};
}