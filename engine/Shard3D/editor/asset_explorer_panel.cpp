#include "../s3dpch.h"  
#include "asset_explorer_panel.h"
#include "../ecs.h"

#include <imgui.h>
#include "../core/asset/assetmgr.h"
#include "imgui_implementation.h"

namespace Shard3D {

	void AssetExplorerPanel::refreshIterator(std::filesystem::path newPath) {
		SHARD3D_NOIMPL;
		directoryEntries.clear();
		for (auto& dirEnt : std::filesystem::directory_iterator(newPath)) {
			directoryEntries.push_back(dirEnt);
		}
	}

	AssetExplorerPanel::AssetExplorerPanel() : currentDir(ENGINE_ASSETS_PATH) { 
		{
			auto& img = _special_assets::_editor_icons.at("editor.browser.folder");
			folderIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file");
			fileIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		refreshIterator(std::filesystem::path(ENGINE_ASSETS_PATH));
	}

	AssetExplorerPanel::~AssetExplorerPanel() {
	}


	void AssetExplorerPanel::render() {
		ImGui::Begin("Asset Explorer");
		if (currentDir != std::filesystem::path(ENGINE_ASSETS_PATH)) {
			if (ImGui::Button("<= Back")) {
				currentDir = currentDir.parent_path();
			}
		}
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = std::max((int)(panelWidth / 112.f)// <--- thumbnail size (96px) + padding (16px)
														, 1);

		ImGui::Columns(columnCount, 0, false);
		for (auto& dirEnt : directoryEntries) {
			const auto& dirEntPath = dirEnt.path();
			auto relativePath = std::filesystem::relative(dirEntPath, std::filesystem::path(ENGINE_ASSETS_PATH));
			std::string fileStr = relativePath.filename().string();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
			ImGui::ImageButton(dirEnt.is_directory() ? folderIcon : fileIcon, { 96.f, 96.f });
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();		
			ImGui::TextWrapped(fileStr.c_str());
			ImGui::NextColumn();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				if (dirEnt.is_directory()) { 
					currentDir /= dirEntPath.filename();
					refreshIterator(currentDir);
					break; 
				}
			}
		}
		ImGui::Columns(1);
		ImGui::End();
	}
}