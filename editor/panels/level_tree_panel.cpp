#include "level_tree_panel.h"

#include <imgui.h>
#include <Shard3D/core/asset/assetmgr.h>
#include <Shard3D/utils/dialogs.h>
namespace Shard3D {

	LevelTreePanel::LevelTreePanel(const sPtr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelTreePanel::~LevelTreePanel() { context = nullptr; }

	void LevelTreePanel::setContext(const sPtr<Level>& levelContext) { context = levelContext; }
	void LevelTreePanel::destroyContext() { context = {}; }
	void LevelTreePanel::clearSelectedActor() { selectedActor = {}; }

	void LevelTreePanel::render() {
		SHARD3D_ASSERT(context != nullptr && "Context not provided!");
		ImGui::Begin(std::string("Level Tree (" + context->name + ")").c_str());
		context->registry.each([&](auto actorGUID) {
			ECS::Actor actor{ actorGUID, context.get() };	
			if (actor.isInvalid()) return;
			drawActorEntry(actor);
		});
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) selectedActor = {};
		
		// blank space RCLICK
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Blank Actor")) selectedActor = context->createActor();
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Camera Actor")) { 
				auto actor = context->createActor("Camera Actor"); 
				actor.addComponent<Components::CameraComponent>(); 
				if (!actor.hasComponent<Components::MeshComponent>()) {
					ResourceHandler::loadMesh(AssetID("assets/_engine/msh/camcord.obj" ENGINE_ASSET_SUFFIX));
					actor.addComponent<Components::MeshComponent>(AssetID("assets/_engine/msh/camcord.obj" ENGINE_ASSET_SUFFIX));
				}
				selectedActor = actor;
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Billboard Actor")) { auto actor = context->createActor("Billboard"); actor.addComponent<Components::BillboardComponent>(AssetID(ENGINE_ERRTEX ENGINE_ASSET_SUFFIX));}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Static Mesh")) { auto actor = context->createActor("Cube"); actor.addComponent<Components::MeshComponent>(AssetID(ENGINE_DEFAULT_MODEL_FILE ENGINE_ASSET_SUFFIX)); }
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