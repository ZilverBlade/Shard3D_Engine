#include "../s3dtpch.h"  
#include "asset_explorer_panel.hpp"
#include "../components.hpp"
#include "../wb3d/bpmgr.hpp"

#include <imgui.h>
#include "../wb3d/assetmgr.hpp"
#include "imgui_implementation.hpp"

namespace Shard3D {
	AssetExplorerPanel::AssetExplorerPanel() : currentDir(ENGINE_ASSETS_PATH) { 
		{
			auto& img = _special_assets::_editor_icons.at("editor.browser.folder");
			folderIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file");
			fileIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	AssetExplorerPanel::~AssetExplorerPanel() {
		//for (auto& assetProperties : assetPropertiesRenderList) {
		//	delete assetProperties.second; // clean up pointers
		//}
		assetPropertiesRenderList.clear();
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
		for (auto& dirEnt : std::filesystem::directory_iterator(currentDir)) {
			const auto& dirEntPath = dirEnt.path();
			auto relativePath = std::filesystem::relative(dirEntPath, std::filesystem::path(ENGINE_ASSETS_PATH));
			std::string fileStr = relativePath.filename().string();
			//if (dirEnt.is_directory()) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
			ImGui::ImageButton(dirEnt.is_directory() ? folderIcon : fileIcon, { 96.f, 96.f });
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
				if (dirEnt.is_directory()) currentDir /= dirEntPath.filename();
				else assetPropertiesRenderList.try_emplace(fileStr, AssetPropertiesPanel(fileStr));
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::TextWrapped(fileStr.c_str());
			//} else {
			//	if (ImGui::Button(fileStr.c_str())) {
			//	}
			//}
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::End();

		for (auto& assetProperties : assetPropertiesRenderList) {
			assetProperties.second.render();
		}
	}
}