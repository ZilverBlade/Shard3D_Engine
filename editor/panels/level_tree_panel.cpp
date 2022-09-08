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
		ImGui::Begin("Level Tree");
		ImGui::Text(context->name.c_str());
		context->registry.each([&](auto actorGUID) {
			ECS::Actor actor{ actorGUID, context.get() };	
			if (actor.isInvalid()) return;
			if (actor.hasComponent<Components::RelationshipComponent>()) if (actor.getComponent<Components::RelationshipComponent>().parentActor != entt::null) return;
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
				if (!actor.hasComponent<Components::Mesh3DComponent>()) {
					ResourceHandler::loadMesh(AssetID("assets/_engine/msh/camcord.obj" ENGINE_ASSET_SUFFIX));
					actor.addComponent<Components::Mesh3DComponent>(AssetID("assets/_engine/msh/camcord.obj" ENGINE_ASSET_SUFFIX));
				}
				selectedActor = actor;
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Billboard Actor")) { auto actor = context->createActor("Billboard"); actor.addComponent<Components::BillboardComponent>(ResourceHandler::coreAssets.t_errorTexture);}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::MenuItem("New Static Mesh")) { auto actor = context->createActor("Cube"); actor.addComponent<Components::Mesh3DComponent>(ResourceHandler::coreAssets.m_defaultModel); }
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
		if (selectedActor)
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Remove Actor")) actorExists = false;
				if (ImGui::MenuItem("Copy UUID to clipboard")) ImGui::SetClipboardText(std::to_string(selectedActor.getUUID()).c_str());
				if (ImGui::MenuItem("Add Child")) {
					context->parentActor(context->createActor(), selectedActor);
				}

				ImGui::EndPopup();
			}

		if (expanded) {
			if (actor.hasComponent<Components::RelationshipComponent>())
				for (auto& child : actor.getComponent<Components::RelationshipComponent>().childActors)
					drawActorEntry({ child, context.get() });
			ImGui::TreePop();
		}
		//if (ImGui::BeginDragDropTarget()) {		
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ACTOR")) {
		//		context->parentActor({ (entt::entity)(*reinterpret_cast<uint32_t*>(payload->Data)), context.get() }, selectedActor);
		//	}
		//}
		//if (ImGui::BeginDragDropSource()) {
		//	uint32_t src = (uint32_t)selectedActor.actorHandle;
		//	ImGui::SetDragDropPayload("SHARD3D.ACTOR", &src, sizeof(uint32_t), ImGuiCond_Once);
		//}
		if (!actorExists) {
			context->killActor(actor);
			if (selectedActor == actor) selectedActor = {};
		}
	}
}