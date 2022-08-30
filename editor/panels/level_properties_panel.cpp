#include <glm/gtc/type_ptr.hpp>
#include "level_properties_panel.h"

#include <Shard3D/ecs.h>

#include <imgui_internal.h>

#include <Shard3D/scripting/dynamic_script_engine.h>
#include <fstream>
namespace Shard3D {
	LevelPropertiesPanel::LevelPropertiesPanel(const sPtr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPropertiesPanel::~LevelPropertiesPanel() {}

	void LevelPropertiesPanel::setContext(const sPtr<Level>& levelContext) { context = levelContext; }
	void LevelPropertiesPanel::destroyContext() { context = {}; }

	void LevelPropertiesPanel::render(LevelTreePanel& tree) {
		SHARD3D_ASSERT(context != nullptr && "Context not provided!");
		ImGui::Begin("Properties"); ImGui::BeginDisabled(context->simulationState == PlayState::Playing/*&& ini.canEditDuringSimulation*/);
		if (tree.selectedActor) { 
			drawActorProperties(tree.selectedActor);
			if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");
			if (ImGui::BeginPopup("AddComponent")) {
				if (!tree.selectedActor.hasComponent<Components::TransformComponent>()) if (ImGui::MenuItem("Transform")) {
					tree.selectedActor.addComponent<Components::TransformComponent>();
					ImGui::CloseCurrentPopup();
				}
				if (!tree.selectedActor.hasComponent<Components::ScriptComponent>()) if (ImGui::MenuItem("Script")) {
					tree.selectedActor.addComponent<Components::ScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
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
					tree.selectedActor.addComponent<Components::MeshComponent>(AssetID(ENGINE_DEFAULT_MODEL_FILE ENGINE_ASSET_SUFFIX));
					ImGui::CloseCurrentPopup();
				}
				if (!tree.selectedActor.hasComponent<Components::BillboardComponent>()) if (ImGui::MenuItem("Billboard")) {
					tree.selectedActor.addComponent<Components::BillboardComponent>(AssetID(ENGINE_ERRTEX));
					ImGui::CloseCurrentPopup();
				}
				if (!tree.selectedActor.hasComponent<Components::AudioComponent>()) if (ImGui::MenuItem("Audio")) {
					tree.selectedActor.addComponent<Components::AudioComponent>();
					ImGui::CloseCurrentPopup();
				}
				if (!tree.selectedActor.hasComponent<Components::CameraComponent>()) if (ImGui::MenuItem("Camera")) {
					tree.selectedActor.addComponent<Components::CameraComponent>();
					ResourceHandler::loadMesh(AssetID("assets/_engine/msh/camcord.obj" ENGINE_ASSET_SUFFIX));
					tree.selectedActor.addComponent<Components::MeshComponent>(AssetID("assets/_engine/msh/camcord.obj" ENGINE_ASSET_SUFFIX));
					tree.selectedActor.getComponent<Components::MeshComponent>().hideInGame = true;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
		if (showPreviewCamera) displayPreviewCamera(tree.selectedActor);
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

	void LevelPropertiesPanel::displayPreviewCamera(Actor& actor) {
		ImGui::Begin("PREVIEW (viewport)");
#if ENSET_ALLOW_PREVIEW_CAMERA
		ImGui::Image(Singleton::previewViewportImage, { 800, 600 });
#endif
		ImGui::End();
	}
	void LevelPropertiesPanel::drawActorProperties(Actor& actor) {
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
				glm::vec3 tran = actor.getComponent<Components::TransformComponent>().getTranslation();
				drawTransformControl("Translation", tran, 0.f);
				actor.getComponent<Components::TransformComponent>().setTranslation(tran);


				glm::vec3 rota = glm::degrees(actor.getComponent<Components::TransformComponent>().getRotation());
				drawTransformControl("Rotation", rota, 0.f, 0.1f);
				actor.getComponent<Components::TransformComponent>().setRotation(glm::radians(rota));

				glm::vec3 scal = actor.getComponent<Components::TransformComponent>().getScale();
				drawTransformControl("Scale", scal, 1.f);
				actor.getComponent<Components::TransformComponent>().setScale(scal);

				ImGui::TreePop();
			}
		}
		if (actor.hasComponent<Components::CppScriptComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::CppScriptComponent).hash_code(), nodeFlags, "C++ (Native) Script");
			ImGui::OpenPopupOnItemClick("KillComponent", ImGuiPopupFlags_MouseButtonRight);
			
			if (open) {
				ImGui::Text(std::to_string(reinterpret_cast<uint64_t>(actor.getComponent<Components::CppScriptComponent>().InstScript)).c_str());
				ImGui::TreePop();
			}
			
		}
		if (actor.hasComponent<Components::ScriptComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::ScriptComponent).hash_code(), nodeFlags, "Script");
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
				bool exists = false;
				static char buffer[32];

				auto& name = actor.getComponent<Components::ScriptComponent>().name;
				memset(buffer, 0, 32);
				strncpy(buffer, name.c_str(), 32);
				//const auto& actorClasses = DynamicScriptEngine::getActorClasses(actor.getComponent<Components::ScriptComponent>().lang);
				exists = DynamicScriptEngine::doesClassExist("Shard3D.Scripts." + name, actor.getComponent<Components::ScriptComponent>().lang);
				if (!exists) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.4, 0.7f, 1.0f));
				if (ImGui::InputText("Script Class", buffer, 32)) {
					name = std::string(buffer);
				}
				if (!exists) ImGui::PopStyleColor();
				ImGui::Combo("Language", &actor.getComponent<Components::ScriptComponent>().lang, "C#\0Visual Basic");
				
				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::ScriptComponent>();
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
				auto& rfile = actor.getComponent<Components::MeshComponent>().asset.getFileRef();
				
				char fileBuffer[256];
				memset(fileBuffer, 0, 256);
				strncpy(fileBuffer, rfile.c_str(), 256);
				
				if (ImGui::InputText("Mesh File", fileBuffer, 256)) {
					rfile = std::string(fileBuffer);			
				}
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.MESH")) {
						rfile = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
					}
				}
				if (ImGui::Button("Load Mesh")) {
					std::ifstream ifile(fileBuffer);
					if (ifile.good()) {
						SHARD3D_LOG("Reloading mesh '{0}'", rfile);
						AssetID meshAsset = AssetID(rfile);
						actor.getComponent<Components::MeshComponent>().asset = meshAsset;
						ResourceHandler::loadMesh(meshAsset);
						actor.getComponent<Components::MeshComponent>() = Components::MeshComponent(meshAsset);
					} else SHARD3D_WARN("File '{0}' does not exist!", fileBuffer);
				}
				for (int i = 0; i < actor.getComponent<Components::MeshComponent>().materials.size(); i++) {
					ImGui::Text("%i# %s", i, ResourceHandler::retrieveMesh(actor.getComponent<Components::MeshComponent>().asset)->materialSlots[i].c_str());
					auto& tag = actor.getComponent<Components::MeshComponent>().materials[i].getFileRef();
					char tagBuffer[256];
					memset(tagBuffer, 0, 256);
					strncpy(tagBuffer, tag.c_str(), 256);
					if (ImGui::InputText("##inputmaterial", tagBuffer, 256)) {}
					if (ImGui::BeginDragDropTarget())
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.SMAT")) {
							tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
							actor.getComponent<Components::MeshComponent>().materials[i] = AssetID(actor.getComponent<Components::MeshComponent>().materials[i].getFile());
						}
				}
				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::MeshComponent>();
		}
		if (actor.hasComponent<Components::BillboardComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::BillboardComponent).hash_code(), nodeFlags, "Billboard");
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
				auto& rfile = actor.getComponent<Components::BillboardComponent>().asset.getFileRef();
			
				char fileBuffer[256];
				memset(fileBuffer, 0, 256);
				strncpy(fileBuffer, rfile.c_str(), 256);
			
				if (ImGui::InputText("Texture File", fileBuffer, 256)) {
					rfile = std::string(fileBuffer);
				}
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						rfile = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
					}
				}
				if (ImGui::Button("Load Texture")) {
					std::ifstream ifile(fileBuffer);
					if (ifile.good()) {
						SHARD3D_LOG("Reloading texture '{0}'", rfile);
						actor.getComponent<Components::BillboardComponent>().asset = AssetID(rfile);
						ResourceHandler::loadTexture(actor.getComponent<Components::BillboardComponent>().asset);
					}
					else SHARD3D_WARN("File '{0}' does not exist!", fileBuffer);
				}
				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::BillboardComponent>();
		}
		if (actor.hasComponent<Components::AudioComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(Components::AudioComponent).hash_code(), nodeFlags, "Audio");
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
				auto& rfile = actor.getComponent<Components::AudioComponent>().file;
				char fileBuffer[256];
				memset(fileBuffer, 0, 256);
				strncpy(fileBuffer, rfile.c_str(), 256);
				if (ImGui::InputText("Audio File", fileBuffer, 256)) {
					rfile = std::string(fileBuffer);
				}
				if (ImGui::TreeNode("Audio Properties")) {
					ImGui::DragFloat("Volume", &actor.getComponent<Components::AudioComponent>().properties.volume, 0.01f, 0.f, 10.f);
					ImGui::DragFloat("Pitch", &actor.getComponent<Components::AudioComponent>().properties.pitch, 0.01f, 0.f, 2.f);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			if (killComponent) actor.killComponent<Components::AudioComponent>();
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
#if ENSET_ALLOW_PREVIEW_CAMERA
				if (ImGui::Button("Preview", { 150, 50 })) {
				//	Singleton::activeLevel->setPossessedPreviewCameraActor(actor);
					context->setPossessedPreviewCameraActor(actor);
					showPreviewCamera = true;
				}
#endif // !DEPLOY
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
				if (ImGui::TreeNode("Attenuation")) {
					ImGui::DragFloat("Constant", &actor.getComponent<Components::PointlightComponent>().attenuationMod.x, 0.005f);
					ImGui::DragFloat("Linear", &actor.getComponent<Components::PointlightComponent>().attenuationMod.y, 0.005f);
					ImGui::DragFloat("Quadratic", &actor.getComponent<Components::PointlightComponent>().attenuationMod.z, 0.005f);
					ImGui::TreePop();
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
		
	}
}
