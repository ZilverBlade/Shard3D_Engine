#include "level_properties_panel.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../engine_window.hpp"

#include <imgui_internal.h>
namespace Shard3D {
	LevelPropertiesPanel::LevelPropertiesPanel(const std::shared_ptr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPropertiesPanel::~LevelPropertiesPanel() {}

	void LevelPropertiesPanel::setContext(const std::shared_ptr<Level>& levelContext) { context = levelContext; }
	void LevelPropertiesPanel::destroyContext() { context = {}; }

	void LevelPropertiesPanel::render(LevelTreePanel tree, EngineDevice* device) {
		ImGui::Begin("Properties");

		if (tree.selectedActor){ drawActorProperties(tree.selectedActor, *device); }
		ImGui::End();
	}


	static void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float stepVal = 0.01f, float columnWidth = 100.f) {
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
		float lineHeight = ENGINE_FONT_SIZE + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.2f, 0.2f, 1.f));
		if (ImGui::Button("X", buttonSize)) values.x = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##X", &values.x, stepVal); ImGui::PopItemWidth(); ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.5f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.7f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.7f, 0.3f, 1.f));
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue;	
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##Y", &values.y, stepVal); ImGui::PopItemWidth(); ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.7f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.9f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.9f, 1.f));
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##Z", &values.z, stepVal); ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}


	void LevelPropertiesPanel::drawActorProperties(Actor actor, EngineDevice& device) {
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
				drawVec3Control("Translation", actor.getComponent<Components::TransformComponent>().translation, 0.f);

				glm::vec3 rot = glm::degrees(actor.getComponent<Components::TransformComponent>().rotation);
				drawVec3Control("Rotation", rot, 0.f, 0.1f);
				actor.getComponent<Components::TransformComponent>().rotation = glm::radians(rot);

				drawVec3Control("Scale", actor.getComponent<Components::TransformComponent>().scale, 1.f);

				ImGui::TreePop();
			}
		}

		if (actor.hasComponent<Components::MeshComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Components::MeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh")) {
				auto& file = actor.getComponent<Components::MeshComponent>().file;
				auto& isMeshIndexedChkBx = actor.getComponent<Components::MeshComponent>().isIndexed;
				char fileBuffer[256];
				memset(fileBuffer, 0, 256);
				strncpy(fileBuffer, file.c_str(), 256);
				if (ImGui::InputText("Mesh File", fileBuffer, 256)) {
					file = std::string(fileBuffer);
				}
				ImGui::Checkbox("Index model", &isMeshIndexedChkBx);
				if (ImGui::Button("(Re)Load Mesh")) {
					std::cout << "guid " << actor.getGUID() << "\n";
					model = actor.getComponent<Components::MeshComponent>().model; //dont index because model breaks

					actor.getComponent<Components::MeshComponent>().reapplyModel(model);
				}

				ImGui::TreePop();
			}
		}
		if (actor.hasComponent<Components::PointlightComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Components::PointlightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Pointlight")) {
				ImGui::ColorEdit3("Color", glm::value_ptr(actor.getComponent<Components::PointlightComponent>().color), ImGuiColorEditFlags_Float);
				ImGui::DragFloat("Intensity", &actor.getComponent<Components::PointlightComponent>().lightIntensity, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Radius", &actor.getComponent<Components::PointlightComponent>().radius, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Specular", &actor.getComponent<Components::PointlightComponent>().specularMod, 0.001f, 0.f, 1.f);
				if (ImGui::CollapsingHeader("Attenuation CLQ")) {
					ImGui::DragFloat("Constant", &actor.getComponent<Components::PointlightComponent>().attenuationMod.x, 0.005f);
					ImGui::DragFloat("Linear", &actor.getComponent<Components::PointlightComponent>().attenuationMod.y, 0.005f);
					ImGui::DragFloat("Quadratic", &actor.getComponent<Components::PointlightComponent>().attenuationMod.z, 0.005f);
				}
				ImGui::TreePop();
			}
		}
		if (actor.hasComponent<Components::SpotlightComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Components::SpotlightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Spotlight")) {
				ImGui::ColorEdit3("Color", glm::value_ptr(actor.getComponent<Components::SpotlightComponent>().color), ImGuiColorEditFlags_Float);
				ImGui::DragFloat("Intensity", &actor.getComponent<Components::SpotlightComponent>().lightIntensity, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Radius", &actor.getComponent<Components::SpotlightComponent>().radius, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Specular", &actor.getComponent<Components::SpotlightComponent>().specularMod, 0.001f, 0.f, 1.f);
				ImGui::TreePop();
			}
		}
		if (actor.hasComponent<Components::DirectionalLightComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Components::DirectionalLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Directional Light")) {
				ImGui::ColorEdit3("Color", glm::value_ptr(actor.getComponent<Components::DirectionalLightComponent>().color), ImGuiColorEditFlags_Float);
				ImGui::DragFloat("Intensity", &actor.getComponent<Components::DirectionalLightComponent>().lightIntensity, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Specular", &actor.getComponent<Components::DirectionalLightComponent>().specularMod, 0.001f, 0.f, 1.f);
				ImGui::TreePop();
			}
		}
	}
}
