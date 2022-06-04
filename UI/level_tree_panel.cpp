#include "level_tree_panel.hpp"
#include "../components.hpp"
#include <imgui.h>
namespace Shard3D {

	LevelTreePanel::LevelTreePanel(const std::shared_ptr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelTreePanel::~LevelTreePanel() {}

	void LevelTreePanel::setContext(const std::shared_ptr<Level>& levelContext) {
		context = levelContext;
	}

	void LevelTreePanel::render() {
		ImGui::Begin("Level Tree");
		
		context->eRegistry.each([&](auto actorGUID) {
			wb3d::Actor actor{ actorGUID, context.get() };	
			if (!actor.hasComponent<Components::TagComponent>()) return;
			if (actor.getGUID() == 0 || actor.getGUID() == std::numeric_limits<uint64_t>::max()) return;	 // dont display these actors as they are engine reserved
			drawActorEntry(actor);
		});
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) selectedActor = {};

		ImGui::End();
	}
	void LevelTreePanel::drawActorEntry(Actor actor) {
		auto& tag = actor.getComponent<Components::TagComponent>().tag;
		ImGuiTreeNodeFlags flags = ((selectedActor == actor) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		bool expanded = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)actor, flags, tag.c_str());
		if (ImGui::IsItemClicked()) selectedActor = actor; 
		if (expanded) {
			ImGui::TreePop();
		}

	}
}