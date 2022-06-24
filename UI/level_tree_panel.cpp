#include "level_tree_panel.hpp"
#include "../components.hpp"
#include "../wb3d/bpmgr.hpp"
#include "../utils/dialogs.h"
#include <imgui.h>
namespace Shard3D {

	LevelTreePanel::LevelTreePanel(const std::shared_ptr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelTreePanel::~LevelTreePanel() { context = nullptr; }

	void LevelTreePanel::setContext(const std::shared_ptr<Level>& levelContext) { context = levelContext; }
	void LevelTreePanel::destroyContext() { context = {}; }
	void LevelTreePanel::clearSelectedActor() { selectedActor = {}; }

	void LevelTreePanel::render() {
		ImGui::Begin("Level Tree");
		
		context->registry.each([&](auto actorGUID) {
			wb3d::Actor actor{ actorGUID, context.get() };	
			if (!actor.hasComponent<Components::TagComponent>()) return;
			if (actor.getGUID() == 0 || actor.getGUID() == UINT64_MAX) return;	 // dont display these actors as they are engine reserved
			drawActorEntry(actor);
		});
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) selectedActor = {};
		
		// blank space RCLICK
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Blank Actor")) selectedActor = context->createActor();
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Camera Actor")) { auto actor = context->createActor("Camera Actor"); actor.addComponent<Components::CameraComponent>(); selectedActor = actor; }//actor.addComponent<Components::MeshComponent>(EngineModel::createModelFromFile(EngineDevice::getDevice(), "assets/modeldata/engineModels/camcord.obj", ModelType::MODEL_TYPE_OBJ, true));
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Pointlight")) { auto actor = context->createActor("Pointlight"); actor.addComponent<Components::PointlightComponent>(); selectedActor = actor; }
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Spotlight")) { auto actor = context->createActor("Spotlight"); actor.addComponent<Components::SpotlightComponent>(); selectedActor = actor; }
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Directional Light")) { auto actor = context->createActor("Directional Light"); actor.addComponent<Components::DirectionalLightComponent>(); selectedActor = actor; }
			ImGui::EndPopup();
		}

		ImGui::End();
	}
	void LevelTreePanel::drawActorEntry(Actor actor) {
		auto& tag = actor.getComponent<Components::TagComponent>().tag;
		ImGuiTreeNodeFlags flags = ((selectedActor == actor) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		bool expanded = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)actor, flags, tag.c_str());
		if (ImGui::IsItemClicked()) selectedActor = actor; 

		// remove stuff
		bool actorExists = true;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Actor")) actorExists = false;
			if (!actor.hasComponent<Components::BlueprintComponent>())
				if (ImGui::MenuItem("Convert to Blueprint")) {
					std::string filepath = FileDialogs::saveFile(WORLDBUILDER3D_ASSETFILE_OPTIONS);
					if (!filepath.empty()) {
						context->createBlueprint(selectedActor, filepath);
					}
				}	
			ImGui::EndPopup();
		}

		if (expanded) {
			ImGui::TreePop();
		}

		if (!actorExists) {
			context->killActor(actor);
			if (selectedActor == actor) selectedActor = {};
		}
	}
}