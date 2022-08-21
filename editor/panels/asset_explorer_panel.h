#pragma once

#include <Shard3D/core/ecs/level.h>
#include <Shard3D/core/ecs/actor.h>
#include <filesystem>

using namespace Shard3D::ECS;
namespace Shard3D {
	class AssetExplorerPanel {
	public:

		AssetExplorerPanel();
		~AssetExplorerPanel();

		void render();
	private:
		VkDescriptorSet folderIcon;
		VkDescriptorSet fileIcon;
		void refreshIterator(std::filesystem::path newPath);
		std::filesystem::path currentDir;
		std::vector<std::filesystem::directory_entry> directoryEntries;
	};
}