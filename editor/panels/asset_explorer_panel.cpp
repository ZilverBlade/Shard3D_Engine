#include "asset_explorer_panel.h"

#include <Shard3D/core/asset/assetmgr.h>
#include <Shard3D/utils/dialogs.h>
#include <fstream>

#include <imgui.h>

#include "../imgui/imgui_implementation.h"
#include <Shard3D/core/asset/matmgr.h>
#include <Shard3D/core/ecs/levelmgr.h>

namespace Shard3D {
	void AssetExplorerPanel::refreshIterator(std::filesystem::path newPath) {
		directoryEntries.entries.clear();
		directoryEntries.types.clear();
		directoryEntries.icons.clear();
		for (auto& dirEnt : std::filesystem::directory_iterator(newPath)) {
			auto relativePath = std::filesystem::relative(dirEnt.path(), std::filesystem::path(ENGINE_ASSETS_PATH));
			std::string fileStr = relativePath.filename().string();
			if (!dirEnt.is_directory() && !(strUtils::hasEnding(fileStr, ENGINE_ASSET_SUFFIX) || strUtils::hasEnding(fileStr, ".s3dlevel"))) continue;
			directoryEntries.entries.push_back(dirEnt);
			if (dirEnt.is_directory()) {
				directoryEntries.types.push_back(AssetType::Unknown);
				directoryEntries.icons.push_back(&folderIcon);
				continue;
			}
			switch (AssetUtils::discoverAssetType("assets\\" + relativePath.string())) {
			case (AssetType::Model3D):
				directoryEntries.types.push_back(AssetType::Model3D);
				directoryEntries.icons.push_back(&mesh3dIcon);
				continue;
			case (AssetType::Texture):
				directoryEntries.types.push_back(AssetType::Texture);
				directoryEntries.icons.push_back(&textureIcon);
				continue;
			case (AssetType::SurfaceMaterial):
				directoryEntries.types.push_back(AssetType::SurfaceMaterial);
				directoryEntries.icons.push_back(&smatIcon);
				continue;
			case (AssetType::PostProcessingMaterial):
				directoryEntries.types.push_back(AssetType::PostProcessingMaterial);
				directoryEntries.icons.push_back(&pmatIcon);
				continue;
			case (AssetType::Level):
				directoryEntries.types.push_back(AssetType::Level);
				directoryEntries.icons.push_back(&levelIcon);
				continue;
			}
			directoryEntries.types.push_back(AssetType::Unknown);
			directoryEntries.icons.push_back(&fileIcon);
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
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file.smt");
			smatIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.browser.file.pmt");
			pmatIcon = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		refreshIterator(std::filesystem::path(ENGINE_ASSETS_PATH));
	}

	AssetExplorerPanel::~AssetExplorerPanel() {}

	void AssetExplorerPanel::render() {
		bool refresh_it = false;

		ImGui::Begin("Asset Explorer");
		if (ImGui::ImageButtonWithText(fileIcon, "Refresh", {16.f, 16.f})) {
			refresh_it = true;
		}
		if (currentDir != std::filesystem::path(ENGINE_ASSETS_PATH)) {
			ImGui::SameLine();
			if (ImGui::Button("<= Back")) {
				currentDir = currentDir.parent_path();
				refresh_it = true;
			}
		}

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = std::max((int)(panelWidth / (96.f + 16.f))// <--- thumbnail size (96px) + padding (16px)
																, 1);

		ImGui::Columns(columnCount, 0, false);
		for (int i = 0; i < directoryEntries.entries.size(); i++) {
			auto& dirEnt = directoryEntries.entries[i];
			const auto& dirEntPath = dirEnt.path();
			auto relativePath = std::filesystem::relative(dirEntPath, std::filesystem::path(ENGINE_ASSETS_PATH));
			std::string fileStr = relativePath.filename().string();
			
			ImGui::PushID(fileStr.data());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);

			ImGui::ImageButton(*directoryEntries.icons[i], { 96.f, 96.f });
			if (ImGui::IsItemHovered()) {
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					if (dirEnt.is_directory()) {
						currentDir /= dirEntPath.filename();
						refresh_it = true;
					}
				}
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					currentContextMenu = dirEntPath;
				}
			}
			if (!dirEnt.is_directory())
				if (ImGui::BeginDragDropSource()) {
					// likely memory leak. if any issues in the future, check this
					char* item = (char*)malloc(relativePath.u8string().length() + 1);
					strncpy(item, const_cast<char*>(relativePath.u8string().c_str()), relativePath.u8string().length() + 1);
					switch (directoryEntries.types[i]) {
					case(AssetType::Texture):
						ImGui::SetDragDropPayload("SHARD3D.ASSEXP.TEX", item, strlen(item) + 1, ImGuiCond_Once);
						break;
					case(AssetType::Model3D):
						ImGui::SetDragDropPayload("SHARD3D.ASSEXP.MESH", item, strlen(item) + 1, ImGuiCond_Once);
						break;
					case(AssetType::Level):
						ImGui::SetDragDropPayload("SHARD3D.ASSEXP.LVL", item, strlen(item) + 1, ImGuiCond_Once);
						break;
					case(AssetType::SurfaceMaterial):
						ImGui::SetDragDropPayload("SHARD3D.ASSEXP.SMAT", item, strlen(item) + 1, ImGuiCond_Once);
						break;
					case(AssetType::PostProcessingMaterial):
						ImGui::SetDragDropPayload("SHARD3D.ASSEXP.PMAT", item, strlen(item) + 1, ImGuiCond_Once);
						break;
					case(AssetType::Unknown):
						ImGui::SetDragDropPayload("SHARD3D.ASSEXP.WHAT", item, strlen(item) + 1, ImGuiCond_Once);
						break;
					}
					ImGui::EndDragDropSource();
				}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();		
			ImGui::TextWrapped(fileStr.c_str());

			ImGui::NextColumn();
			ImGui::PopID();

			if (refresh_it) { refreshIterator(currentDir); break; }
		}
		ImGui::Columns(1);

		if (ImGui::BeginPopupContextWindow("AssetMenu", ImGuiMouseButton_Right, true)) {
			if (ImGui::MenuItem("Rename")) {
				SHARD3D_NOIMPL;

				currentContextMenu = std::filesystem::path();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Replace")) {
				SHARD3D_NOIMPL;

				currentContextMenu = std::filesystem::path();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Delete")) {
				AssetManager::purgeAsset(currentContextMenu.string());
				refresh_it = true;

				currentContextMenu = std::filesystem::path();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow("AssetExplorerContext", ImGuiMouseButton_Right, false)) {
			if (ImGui::MenuItem("Import Texture")) {
				std::string file = FileDialogs::openFile();
				std::ifstream check(file);
				if (check.good()) {
					std::string dest = std::string(currentDir.string() + AssetUtils::truncatePath(file));
					std::replace(dest.begin(), dest.end(), '\\', '/');
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
					std::string dest = std::string(currentDir.string() + AssetUtils::truncatePath(file));
					std::replace(dest.begin(), dest.end(), '\\', '/');
					AssetManager::importMesh(file, dest, Model3DLoadInfo());
					refreshIterator(currentDir);
				}
				else {
					MessageDialogs::show("Invalid model file provided", "Asset Error", MessageDialogs::OPTICONERROR);
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("New level")) {
				LevelManager lman(make_sPtr<Level>("Some kind of level"));
				lman.save(std::string(currentDir.string() + "/Some kind of level"));
				refreshIterator(currentDir);	
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::BeginMenu("Create Material")) {
				if (ImGui::MenuItem("Surface Shaded")) {
					rPtr<SurfaceMaterial_Shaded> blank = make_rPtr<SurfaceMaterial_Shaded>();
					std::string dest = std::string(currentDir.string() + "/Some kind of material");
					AssetManager::createMaterial(dest, blank);
					refreshIterator(currentDir);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Post Processing")) {
					rPtr<PostProcessingMaterial> blank = make_rPtr<PostProcessingMaterial>();
					blank->addParameter(0, "MyParam0");
					std::string dest = std::string(currentDir.string() + "/Some kind of material");
					AssetManager::createMaterial(dest, blank);
					refreshIterator(currentDir);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::End();
	}
}