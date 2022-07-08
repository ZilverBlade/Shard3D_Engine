#include "../s3dtpch.h"
#include "level_peekers.hpp"
#include "../components.hpp"
#include "../wb3d/bpmgr.hpp"

#include <imgui.h>
#include "../wb3d/assetmgr.hpp"
#include "../singleton.hpp"
namespace Shard3D {

	LevelPeekingPanel::LevelPeekingPanel(const std::shared_ptr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPeekingPanel::~LevelPeekingPanel() { context = nullptr; }

	void LevelPeekingPanel::setContext(const std::shared_ptr<Level>& levelContext) { context = levelContext; }
	void LevelPeekingPanel::destroyContext() { context = {}; }

	void LevelPeekingPanel::render() {
		ImGui::Begin("Level Peeker");
		ImGui::Checkbox("Actor Inspector", &actorInspector);
		ImGui::Separator();
		ImGui::Checkbox("LOD Inspector", &lodInspector);
		ImGui::Checkbox("Texture Inspector", &textureInspector);
		ImGui::Separator();
		ImGui::Checkbox("Material Inspector", &materialInspector);
		ImGui::Separator();
		ImGui::Checkbox("Miscellaneous Inspector", &miscInspector);
		ImGui::End();

		if (actorInspector) peekActorInspector();
		if (lodInspector) peekLODInspector();
		if (textureInspector) peekTextureInspector();
		if (materialInspector) peekMaterialInspector();
		if (miscInspector) peekMisc();
	}

	void LevelPeekingPanel::peekMaterialInspector() {
		ImGui::Begin("Material Inspector");
		for (auto i : AssetManager::getMaterialAssets()) {
			std::string text = i.second.materialTag + " (" + std::to_string((uint64_t)i.first) + ")";
			ImGui::Text(text.c_str());
		}
		ImGui::End();
	}

	void LevelPeekingPanel::peekActorInspector() {
		ImGui::Begin("Actor Inspector");
		Singleton::activeLevel->registry.each([&](auto actorGUID) {
			wb3d::Actor actor{ actorGUID, Singleton::activeLevel.get() };
			
			std::string text = actor.getTag() + " (" + std::to_string((uint64_t)actor.getGUID()) + ")";
			if (actor.isInvalid())
				ImGui::TextColored({1.f, 0.f, 1.f, 1.f}, text.c_str());
			else
				ImGui::Text(text.c_str());
		});
		ImGui::End();
	}

	void LevelPeekingPanel::peekTextureInspector() {
		ImGui::Begin("Texture Inspector");
		ImGui::Text((std::string("Texture count: " + std::to_string(AssetManager::getTextureAssets().size())).c_str()));
		if (ImGui::Button("Force delete all textures (dangerous)")) {
			if (MessageDialogs::show(std::string("Deletion of all assets can be dangerous if assets are in use.\nUnexpected crashes may happen if one of these assets is in use.\nAre you sure you want to proceed ?").c_str(), "Caution!", MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTYESNO) == MessageDialogs::RESYES) {
				AssetManager::clearTextureAssets();
			}
		}
		for (const auto& tex : AssetManager::getTextureAssets())
			if (ImGui::TreeNodeEx(std::string(tex.first + "##" + tex.first).c_str(), ImGuiTreeNodeFlags_None)) {}
		ImGui::End();
	}
	void LevelPeekingPanel::peekLODInspector() {
		ImGui::Begin("Mesh & LOD Inspector");
		ImGui::Text((std::string("Mesh count: " + std::to_string(AssetManager::getMeshAssets().size())).c_str()));
		if (ImGui::Button("Force delete all models (dangerous)")) {
			if (MessageDialogs::show(std::string("Deletion of all assets can be dangerous if assets are in use.\nUnexpected crashes may happen if one of these assets is in use.\nAre you sure you want to proceed ?").c_str(), "Caution!", MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTYESNO) == MessageDialogs::RESYES) {
				AssetManager::clearMeshAssets();
			}
		}
		for (const auto& model : AssetManager::getMeshAssets()) 
			if (ImGui::TreeNodeEx(std::string(model.first + "##" + model.first).c_str(), ImGuiTreeNodeFlags_None)) {}
		ImGui::End();
	}

	void LevelPeekingPanel::peekMisc() {
		ImGui::Begin("Misc Inspector");
		ImGui::Text(std::string("Active level ptr: 0x" + std::to_string((int)Singleton::activeLevel.get())).c_str());

		ImGui::Text(std::string("Possessed camera actor: " + Singleton::activeLevel->getPossessedCameraActor().getTag() + " (0x" + std::to_string((int)&Singleton::activeLevel->getPossessedCamera()) + ")").c_str());
#if ALLOW_PREVIEW_CAMERA
		ImGui::Text(std::string("Previewing camera: " + Singleton::activeLevel->getPossessedPreviewCameraActor().getTag() + " (0x" + std::to_string((int)&Singleton::activeLevel->getPossessedPreviewCamera()) + ")").c_str());
#endif
		ImGui::End();
	}
	
}