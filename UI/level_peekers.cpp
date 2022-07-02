#include "level_peekers.hpp"
#include "../components.hpp"
#include "../wb3d/bpmgr.hpp"
#include "../utils/dialogs.h"
#include <imgui.h>
#include "../wb3d/assetmgr.hpp"
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
		ImGui::End();

		if (actorInspector) peekActorInspector();
		if (lodInspector) peekLODInspector();
		if (materialInspector) peekMaterialInspector();
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
			wb3d::Actor actor{ actorGUID, context.get() };
			if (actor.isInvalid()) return;
			std::string text = actor.getTag() + " (" + std::to_string((uint64_t)actor.getGUID()) + ")";
			ImGui::Text(text.c_str());
		});
		ImGui::End();
	}

	void LevelPeekingPanel::peekLODInspector() {
		ImGui::Begin("Model & LOD Inspector");
		context->registry.view<Components::MeshComponent>().each([&](auto mesh) {
			if (ImGui::CollapsingHeader(mesh.file.c_str(), ImGuiTreeNodeFlags_None)) {

			}
		});
		ImGui::End();
	}
	
}