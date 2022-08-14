#include "../s3dpch.h"
#include <imgui.h>
#include "level_peekers.h"
#include "../ecs.h"
#include "../core/asset/assetmgr.h"

namespace Shard3D {

	LevelPeekingPanel::LevelPeekingPanel(const sPtr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPeekingPanel::~LevelPeekingPanel() { context = nullptr; }

	void LevelPeekingPanel::setContext(const sPtr<Level>& levelContext) { context = levelContext; }
	void LevelPeekingPanel::destroyContext() { context = {}; }

	void LevelPeekingPanel::render() {
		assert(context != nullptr && "Context not provided!");
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

		if (!actorInspector) peekActorInspector();
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
		context->registry.each([&](auto actorGUID) {
			ECS::Actor actor{ actorGUID, context.get() };

			std::string text = actor.getTag() + " (" + std::to_string((uint64_t)actor.getUUID()) + ")" 
				+ " handle (" + std::to_string((uint32_t)actor.actorHandle) + ")"
				+ " p_handle (" + std::to_string((uint32_t)context->getParent(actor).actorHandle) + ")";
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
		ImGui::Text(std::string(tex.first).c_str());
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
		ImGui::Text(std::string(model.first).c_str());
		ImGui::End();
	}

	void LevelPeekingPanel::peekMisc() {
		ImGui::Begin("Misc Inspector");
		ImGui::Text(std::string("Active level ("+ context->name+") ptr: 0x" + std::to_string((int)context.get())).c_str());

		ImGui::Text(std::string("Possessed camera actor: " + context->getPossessedCameraActor().getTag() + " (0x" + std::to_string((int)&context->getPossessedCamera()) + ")").c_str());
#if ENSET_ALLOW_PREVIEW_CAMERA
		ImGui::Text(std::string("Previewing camera: " + Singleton::activeLevel->getPossessedPreviewCameraActor().getTag() + " (0x" + std::to_string((int)&Singleton::activeLevel->getPossessedPreviewCamera()) + ")").c_str());
#endif
		ImGui::End();
	}
	
}