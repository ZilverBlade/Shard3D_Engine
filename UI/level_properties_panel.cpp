#include "level_properties_panel.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../engine_window.hpp"
#include <fstream>
#include <imgui_internal.h>
namespace Shard3D {
	LevelPropertiesPanel::LevelPropertiesPanel(const std::shared_ptr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPropertiesPanel::~LevelPropertiesPanel() {}

	void LevelPropertiesPanel::setContext(const std::shared_ptr<Level>& levelContext) { context = levelContext; }
	void LevelPropertiesPanel::destroyContext() { context = {}; }

	void LevelPropertiesPanel::render(LevelTreePanel tree, EngineDevice* device) {
		ImGui::Begin("Properties"); ImGui::BeginDisabled(context->simulationState == PlayState::Simulating/*&& ini.canEditDuringSimulation*/);
		if (tree.selectedActor){ 
			if (!tree.selectedActor.hasComponent<Components::BlueprintComponent>()) {
				drawActorProperties(tree.selectedActor, *device);
				if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");
				if (ImGui::BeginPopup("AddComponent")) {
#if !ACTOR_FORCE_TRANSFORM_COMPONENT
					if (!tree.selectedActor.hasComponent<Components::TransformComponent>()) if (ImGui::MenuItem("Transform")) {
						tree.selectedActor.addComponent<Components::TransformComponent>();
						ImGui::CloseCurrentPopup();
					}
#endif
					if (!tree.selectedActor.hasComponent<Components::PointlightComponent>()) if (ImGui::MenuItem("Pointlight")) {
						tree.selectedActor.addComponent<Components::PointlightComponent>();
						ImGui::CloseCurrentPopup();
					}
					if (!tree.selectedActor.hasComponent<Components::SpotlightComponent>()) if (ImGui::MenuItem("Spotlight")) {
						tree.selectedActor.addComponent<Components::SpotlightComponent>();
						ImGui::CloseCurrentPopup();
					}
					if (!tree.selectedActor.hasComponent<Components::DirectionalLightComponent>()) if (ImGui::MenuItem("Directional Light")) {
						tree.selectedActor.addComponent<Components::DirectionalLightComponent>();
						ImGui::CloseCurrentPopup();
					}
					if (!tree.selectedActor.hasComponent<Components::MeshComponent>()) if (ImGui::MenuItem("Mesh")) {
						//add a default obj
						tree.selectedActor.addComponent<Components::MeshComponent>(EngineModel::createModelFromFile(*device, DEFAULT_MODEL_FILE, ModelType::MODEL_TYPE_OBJ, true));

						ImGui::CloseCurrentPopup();
					}
					if (!tree.selectedActor.hasComponent<Components::CameraComponent>()) if (ImGui::MenuItem("Camera")) {
						tree.selectedActor.addComponent<Components::CameraComponent>();
						tree.selectedActor.addComponent<Components::MeshComponent>(EngineModel::createModelFromFile(*device, "assets/modeldata/engineModels/camcord.obj", ModelType::MODEL_TYPE_OBJ, true));

						ImGui::CloseCurrentPopup();
					}
					if (!tree.selectedActor.hasComponent<Components::CppScriptComponent>()) if (ImGui::MenuItem("C++ Script")) {
						tree.selectedActor.addComponent<Components::CppScriptComponent>();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			else { drawBlueprintInfo(tree.selectedActor); }

			if (ImGui::Button("Actor GUID to clipboard")) {
				ImGui::SetClipboardText(std::to_string(tree.selectedActor.getGUID()).c_str());
				SHARD3D_INFO("copied {0} to clipboard", std::to_string(tree.selectedActor.getGUID()).c_str());
			}
		}
		ImGui::EndDisabled(); ImGui::End();
	}


	static void drawTransformControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float stepVal = 0.01f, float columnWidth = 100.f) {
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
		if (ImGui::Button("Y", buttonSize)) values.z = resetValue;	
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##Y", &values.z, stepVal); ImGui::PopItemWidth(); ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.7f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.9f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.9f, 1.f));
		if (ImGui::Button("Z", buttonSize)) values.y = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##Z", &values.y, stepVal); ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}

	void LevelPropertiesPanel::drawBlueprintInfo(Actor actor) {
		std::string info = "Blueprint asset location: ";//  + actor.getComponent<Components::BlueprintComponent>().blueprint;
		ImGui::TextColored(ImVec4(0.2f, 0.5f, 0.95f, 1.f), info.c_str());
		ImGui::Separator;
		if (ImGui::Button("Blueprint GUID to clipboard")) {
			ImGui::SetClipboardText("id.id");
			SHARD3D_INFO("copied id.id to clipboard");
		}
		ImGui::Separator;
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
			if (ImGui::TreeNodeEx((void*)typeid(Components::TransformComponent).hash_code(), nodeFlags, "Transform")) {
				drawTransformControl("Translation", actor.getComponent<Components::TransformComponent>().translation, 0.f);

				glm::vec3 rot = glm::degrees(actor.getComponent<Components::TransformComponent>().rotation);
				drawTransformControl("Rotation", rot, 0.f, 0.1f);
				actor.getComponent<Components::TransformComponent>().rotation = glm::radians(rot);

				drawTransformControl("Scale", actor.getComponent<Components::TransformComponent>().scale, 1.f);

				ImGui::TreePop();
			}
		}	
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
			if (ImGui::TreeNodeEx((void*)typeid(Components::TransformComponent).hash_code(), nodeFlags, "Transform")) {
				drawTransformControl("Translation", actor.getComponent<Components::TransformComponent>().translation, 0.f);

				glm::vec3 rot = glm::degrees(actor.getComponent<Components::TransformComponent>().rotation);
				drawTransformControl("Rotation", rot, 0.f, 0.1f);
				actor.getComponent<Components::TransformComponent>().rotation = glm::radians(rot);

				drawTransformControl("Scale", actor.getComponent<Components::TransformComponent>().scale, 1.f);

				ImGui::TreePop();
			}
		}
		if (actor.hasComponent<Components::MeshComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::MeshComponent).hash_code(), nodeFlags, "Mesh");
			ImGui::OpenPopupOnItemClick("KillComponent", ImGuiPopupFlags_MouseButtonRight);
			bool killComponent = false;
			if (ImGui::BeginPopup("KillComponent")) {
				if (ImGui::MenuItem("Remove Component")) {
					killComponent = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (open) {
				auto& file = actor.getComponent<Components::MeshComponent>().file;
				char fileBuffer[256];
				memset(fileBuffer, 0, 256);
				strncpy(fileBuffer, file.c_str(), 256);
				if (ImGui::InputText("Mesh File", fileBuffer, 256)) {
					file = std::string(fileBuffer);
				}
				if (ImGui::Button("(Re)Load Mesh")) {
					std::ifstream ifile(file);
					if (ifile.good()) {
						actor.getComponent<Components::MeshComponent>().newModel = EngineModel::createModelFromFile(device,
							actor.getComponent<Components::MeshComponent>().file,
							actor.getComponent<Components::MeshComponent>().type,
							actor.getComponent<Components::MeshComponent>().isIndexed
						);
						context->reloadMesh(actor);
					} else SHARD3D_WARN("File '{0}' does not exist!", fileBuffer);
				}
				ImGui::TreePop();
			}
			if (killComponent) context->killMesh(actor);
		}
		if (actor.hasComponent<Components::CameraComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::CameraComponent).hash_code(), nodeFlags, "Camera");
			ImGui::OpenPopupOnItemClick("KillComponent", ImGuiPopupFlags_MouseButtonRight);
			bool killComponent = false;
			if (ImGui::BeginPopup("KillComponent")) {
				if (ImGui::MenuItem("Remove Component")) {
					killComponent = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (open) {
				int projType = (int)actor.getComponent<Components::CameraComponent>().projectionType;
				ImGui::Combo("Projection Type", &projType, "Orthographic\0Perspective");
				actor.getComponent<Components::CameraComponent>().projectionType = (Components::CameraComponent::ProjectType)projType;
				
				bool enableFov = false;
				if (actor.getComponent<Components::CameraComponent>().projectionType == Components::CameraComponent::ProjectType::Perspective) enableFov = true;
				ImGui::BeginDisabled(!enableFov); ImGui::DragFloat("FOV", &actor.getComponent<Components::CameraComponent>().fov, 0.1f, 10.f, 180.f); ImGui::EndDisabled();
				ImGui::DragFloat("Near Clip Plane", &actor.getComponent<Components::CameraComponent>().nearClip, 0.001f, 0.05f, 1.f);
				ImGui::DragFloat("Far Clip Plane", &actor.getComponent<Components::CameraComponent>().farClip, 1.f, 16.f, 8192.f);
				ImGui::DragFloat("Aspect ratio", &actor.getComponent<Components::CameraComponent>().ar, 0.001f, 0.f, 4.f);

				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::CameraComponent>();
		}
		if (actor.hasComponent<Components::PointlightComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::PointlightComponent).hash_code(), nodeFlags, "Pointlight");
			ImGui::OpenPopupOnItemClick("KillComponent", ImGuiPopupFlags_MouseButtonRight);
			bool killComponent = false;
			if (ImGui::BeginPopup("KillComponent")) {
				if (ImGui::MenuItem("Remove Component")) {
					killComponent = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (open) {		
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
			if (killComponent) actor.killComponent<Components::PointlightComponent>();
		}
		if (actor.hasComponent<Components::SpotlightComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::SpotlightComponent).hash_code(), nodeFlags, "Spotlight");
			ImGui::OpenPopupOnItemClick("KillComponent", ImGuiPopupFlags_MouseButtonRight);
			bool killComponent = false;
			if (ImGui::BeginPopup("KillComponent")) {
				if (ImGui::MenuItem("Remove Component")) {
					killComponent = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (open) {
				ImGui::ColorEdit3("Color", glm::value_ptr(actor.getComponent<Components::SpotlightComponent>().color), ImGuiColorEditFlags_Float);
				ImGui::DragFloat("Intensity", &actor.getComponent<Components::SpotlightComponent>().lightIntensity, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Radius", &actor.getComponent<Components::SpotlightComponent>().radius, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Specular", &actor.getComponent<Components::SpotlightComponent>().specularMod, 0.001f, 0.f, 1.f);
				
				float oa = glm::degrees(actor.getComponent<Components::SpotlightComponent>().outerAngle);
				ImGui::DragFloat("Outer Angle", &oa, 0.1f, 5.f, 90.f);
				actor.getComponent<Components::SpotlightComponent>().outerAngle = glm::radians(oa);
				float ia = glm::degrees(actor.getComponent<Components::SpotlightComponent>().innerAngle);
				ImGui::DragFloat("Inner Angle", &ia, 0.1f, 5.f, 90.f);
				actor.getComponent<Components::SpotlightComponent>().innerAngle = glm::radians(ia);
				
				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::SpotlightComponent>();
		}
		if (actor.hasComponent<Components::DirectionalLightComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::DirectionalLightComponent).hash_code(), nodeFlags, "Directional Light");
			ImGui::OpenPopupOnItemClick("KillComponent", ImGuiPopupFlags_MouseButtonRight);
			bool killComponent = false;
			if (ImGui::BeginPopup("KillComponent")) {
				if (ImGui::MenuItem("Remove Component")) {
					killComponent = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (open) {
				ImGui::ColorEdit3("Color", glm::value_ptr(actor.getComponent<Components::DirectionalLightComponent>().color), ImGuiColorEditFlags_Float);
				ImGui::DragFloat("Intensity", &actor.getComponent<Components::DirectionalLightComponent>().lightIntensity, 0.01f, 0.f, 100.f);
				ImGui::DragFloat("Specular", &actor.getComponent<Components::DirectionalLightComponent>().specularMod, 0.001f, 0.f, 1.f);
				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::DirectionalLightComponent>();
		}
		if (actor.hasComponent<Components::CppScriptComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::CppScriptComponent).hash_code(), nodeFlags, "C++ Script");
			ImGui::OpenPopupOnItemClick("KillComponent", ImGuiPopupFlags_MouseButtonRight);
			bool killComponent = false;
			if (ImGui::BeginPopup("KillComponent")) {
				if (ImGui::MenuItem("Remove Component")) {
					killComponent = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (open) {
				auto& className = actor.getComponent<Components::CppScriptComponent>().className;
				char classNameBuffer[256];
				memset(classNameBuffer, 0, 256);
				strncpy(classNameBuffer, className.c_str(), 256);
				ImGui::Text(typeid(&actor.getComponent<Components::CppScriptComponent>().Inst).name());
				ImGui::Text("Script class:");
				ImGui::Text("Script::"); ImGui::SameLine; ImGui::InputText("", classNameBuffer, 256);
				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::CppScriptComponent>();
		}
	}
}
