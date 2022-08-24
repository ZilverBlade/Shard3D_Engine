#include "asset_explorer_panel.h"
#include <Shard3D/ecs.h>
#include <Shard3D/core/asset/assetmgr.h>
#include <Shard3D/utils/dialogs.h>
#include <fstream>

#include <imgui.h>

#include "../imgui/imgui_implementation.h"

namespace Shard3D {
	void AssetExplorerPanel::refreshIterator(std::filesystem::path newPath) {
		directoryEntries.entries.clear();
		directoryEntries.types.clear();
		for (auto& dirEnt : std::filesystem::directory_iterator(newPath)) {
			auto relativePath = std::filesystem::relative(dirEnt.path(), std::filesystem::path(ENGINE_ASSETS_PATH));
			std::string fileStr = relativePath.filename().string();
			if (!dirEnt.is_directory() && !(strUtils::hasEnding(fileStr, ENGINE_ASSET_SUFFIX) || strUtils::hasEnding(fileStr, ".s3dlevel"))) continue;
			directoryEntries.entries.push_back(dirEnt);
			if (dirEnt.is_directory()) {
				directoryEntries.types.push_back(&folderIcon);
				continue;
			}
			switch (AssetUtils::discoverAssetType(fileStr)) {
			case (AssetType::Mesh3D):
				directoryEntries.types.push_back(&mesh3dIcon);
				continue;
			case (AssetType::Texture):
				directoryEntries.types.push_back(&textureIcon);
				continue;
			case (AssetType::Level):
				directoryEntries.types.push_back(&levelIcon);
				continue;
			}
			directoryEntries.types.push_back(&fileIcon);
		}
	}

	AssetExplorerPanel::AssetExplorerPanel() : currentDir(ENGINE_ASSETS_PATH) { 
		{
			auto& img = _special_assets::_editor_icons.at("editor.browser.folder");
			folderIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file");
			fileIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file.lvl");
			levelIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file.tex");
			textureIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file.msh");
			mesh3dIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		refreshIterator(std::filesystem::path(ENGINE_ASSETS_PATH));
	}

	AssetExplorerPanel::~AssetExplorerPanel() {
	}

	void AssetExplorerPanel::render() {
		bool refresh_it = false;

		ImGui::Begin("Asset Explorer");
		
		if (currentDir != std::filesystem::path(ENGINE_ASSETS_PATH)) {
			if (ImGui::Button("<= Back")) {
				currentDir = currentDir.parent_path();
				refresh_it = true;
			}
		}

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = std::max((int)(panelWidth / 112.f)// <--- thumbnail size (96px) + padding (16px)
														, 1);

		ImGui::Columns(columnCount, 0, false);
		for (int i = 0; i < directoryEntries.entries.size(); i++) {
			auto& dirEnt = directoryEntries.entries[i];
			const auto& dirEntPath = dirEnt.path();
			auto relativePath = std::filesystem::relative(dirEntPath, std::filesystem::path(ENGINE_ASSETS_PATH));
			std::string fileStr = relativePath.filename().string();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
			ImGui::ImageButton(*directoryEntries.types[i], { 96.f, 96.f });
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				if (dirEnt.is_directory()) {
					currentDir /= dirEntPath.filename();
					refresh_it = true;
				}
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();		
			ImGui::TextWrapped(fileStr.c_str());
			ImGui::NextColumn();

			if (refresh_it) { refreshIterator(currentDir); break; }
		}
		ImGui::Columns(1);

		if (ImGui::BeginPopupContextWindow("Import Asset", ImGuiMouseButton_Right, false)) {
			if (ImGui::MenuItem("Import Texture")) {
				std::string file = FileDialogs::openFile();
				std::ifstream check(file);
				if (check.good()) {
					const auto& dest = std::string(currentDir.string() + AssetUtils::truncatePath(file));
					AssetManager::importTexture(file, dest, TextureLoadInfo());
					refreshIterator(currentDir);
				}
				else {
					MessageDialogs::show("Invalid texture file provided", "Asset Error", MessageDialogs::OPTICONERROR);
				}
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Import 3D Model")) {
				std::string file = FileDialogs::openFile();
				std::ifstream check(file);
				if (check.good()) {
					const auto& dest = std::string(currentDir.string() + AssetUtils::truncatePath(file));
					AssetManager::importMesh(file, dest, MeshLoadInfo());
					refreshIterator(currentDir);
				}
				else {
					MessageDialogs::show("Invalid model file provided", "Asset Error", MessageDialogs::OPTICONERROR);
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}


		ImGui::End();
	}
}