#include "level_properties_panel.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Shard3D {
	LevelPropertiesPanel::LevelPropertiesPanel(const std::shared_ptr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPropertiesPanel::~LevelPropertiesPanel() {}

	void LevelPropertiesPanel::setContext(const std::shared_ptr<Level>& levelContext) { context = levelContext; }
	void LevelPropertiesPanel::destroyContext() { context = {}; }

	void LevelPropertiesPanel::render(LevelTreePanel tree) {
		ImGui::Begin("Properties");

		//if (tree.selectedActor){ drawActorProperties(tree.selectedActor); }
		ImGui::End();
	}
	void LevelPropertiesPanel::drawActorProperties(Actor actor) {
		if (actor.hasComponent<Components::TagComponent>()) {
			auto& tag = actor.getComponent<Components::TagComponent>().tag;

			char tagBuffer[256];
			memset(tagBuffer, 0, 256);
			strncpy(tagBuffer, tag.c_str(), 256);
			if (ImGui::InputText("Tag", tagBuffer, 256)) {
				tag = std::string(tagBuffer);
			}
		}
		if (actor.hasComponent<Components::TransformComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Components::TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
				ImGui::DragFloat3("Translation", glm::value_ptr(actor.getComponent<Components::TransformComponent>().translation), 0.01f);
				ImGui::DragFloat3("Rotation", glm::value_ptr(actor.getComponent<Components::TransformComponent>().rotation), 0.01f);
				ImGui::DragFloat3("Scale", glm::value_ptr(actor.getComponent<Components::TransformComponent>().scale), 0.01f);

				ImGui::TreePop();
			}
		}
	}
}
