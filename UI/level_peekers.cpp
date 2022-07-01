#include "level_peekers.hpp"
#include "../components.hpp"
#include "../wb3d/bpmgr.hpp"
#include "../utils/dialogs.h"
#include <imgui.h>
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
	
}