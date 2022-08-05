#include "../s3dtpch.h" 

#include "hud_builder_panel.hpp"
#include <imgui_internal.h>
#include <imgui.h>
#include "../wb3d/hudmgr.hpp"
#include "../utils/definitions.hpp"
#include "../guid.hpp"
#include "../wb3d/assetmgr.hpp"
#include <fstream>
#include "../scripts/dynamic_script_engine.hpp"
namespace Shard3D {
	//HUDBuilderPanel::HUDBuilderPanel() {}

	HUDBuilderPanel::~HUDBuilderPanel() {}

    void HUDBuilderPanel::setContext(std::shared_ptr<HUDContainer> gctnr) {
        hudLayerInfo = gctnr;
    }

    static void drawTransform2DControl(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float stepVal = 0.01f, float columnWidth = 100.f) {
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
        ImGui::SameLine(); ImGui::DragFloat("##Y", &values.y, stepVal); ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();
    }

	void HUDBuilderPanel::render() {
        renderGUIBuilder();
        renderGUIBuilderToolbar();
	}
    void HUDBuilderPanel::renderGUIBuilder() {
        //ImGui::Begin("HUD Builder2D");
        //
        //
        //ImGui::End();
    }
    void HUDBuilderPanel::renderGUIBuilderToolbar() {
        ImGui::Begin("HUD List");
        for (int i = 0; i < hudLayerInfo->getList().size(); i++) {
            if (ImGui::TreeNode(std::string("Layer " + std::to_string(i)).c_str())) {
                if (ImGui::Button("Save Layer")) {
                    wb3d::HUDManager guiman(hudLayerInfo.get());
                    guiman.save("assets/huddata/test.wbgt", i);
                }
                if (ImGui::Button("Load Layer")) {
                    wb3d::HUDManager guiman(hudLayerInfo.get());
                    guiman.load("assets/huddata/test.wbgt", i);
                }
                if (ImGui::Button("Wipe Layer")) {
                    if (MessageDialogs::show("This will delete everything in this layer. Are you sure you want to continue?", "Warning!", MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTYESNO | MessageDialogs::OPTDEFBUTTON2) == MessageDialogs::RESYES) hudLayerInfo->wipe(i);
                }
                if (ImGui::Button("Add Element")) {
                    std::shared_ptr<HUDElement> hudElement = std::make_shared<HUDElement>();
                    hudElement->guid = GUID();
                    hudElement->scale = { 0.1f, 0.1f };
                    hudLayerInfo->getList().at(i)->elements.emplace(hudElement->guid, hudElement);
                }
                for (auto& hudEl : hudLayerInfo->getList().at(i)->elements) {
                    renderGUIElementProperties(hudEl.second);
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void HUDBuilderPanel::renderGUIElementProperties(std::shared_ptr<HUDElement> element) {
        if (ImGui::TreeNodeEx((void*)(element.get()), nodeFlags, element->tag.c_str())) {
            {
                auto& tag = element->tag;
                char tagBuffer[256];
                memset(tagBuffer, 0, 256);
                strncpy(tagBuffer, tag.c_str(), 256);
                if (ImGui::InputText("Tag", tagBuffer, 256)) {
                    tag = std::string(tagBuffer);
                }
            }
            if (ImGui::TreeNode("Textures")) {
                {
                    auto& tag = element->default_texture;
                    char tagBuffer[256];
                    memset(tagBuffer, 0, 256);
                    strncpy(tagBuffer, tag.c_str(), 256);
                    if (ImGui::InputText("Default", tagBuffer, 256)) {
                        tag = std::string(tagBuffer);
                    }

                    if (ImGui::Button("Load Texture##X")) {
                        std::ifstream ifile(tag);
                        if (ifile.good()) {
                            SHARD3D_LOG("Reloading texture '{0}'", tag);
                            wb3d::AssetManager::emplaceTexture(tag);
                            element->default_texture = tag;
                        }
                        else SHARD3D_WARN("File '{0}' does not exist!", tag);
                    }
                }
                {
                    auto& tag = element->hover_texture;
                    char tagBuffer[256];
                    memset(tagBuffer, 0, 256);
                    strncpy(tagBuffer, tag.c_str(), 256);
                    if (ImGui::InputText("Hover", tagBuffer, 256)) {
                        tag = std::string(tagBuffer);
                    }

                    if (ImGui::Button("Load Texture##Y")) {
                        std::ifstream ifile(tag);
                        if (ifile.good()) {
                            SHARD3D_LOG("Reloading texture '{0}'", tag);
                            wb3d::AssetManager::emplaceTexture(tag);
                            element->hover_texture = tag;
                        }
                        else SHARD3D_WARN("File '{0}' does not exist!", tag);
                    }
                }
                {
                    auto& tag = element->press_texture;
                    char tagBuffer[256];
                    memset(tagBuffer, 0, 256);
                    strncpy(tagBuffer, tag.c_str(), 256);
                    if (ImGui::InputText("Press", tagBuffer, 256)) {
                        tag = std::string(tagBuffer);
                    }

                    if (ImGui::Button("Load Texture##Z")) {
                        std::ifstream ifile(tag);
                        if (ifile.good()) {
                            SHARD3D_LOG("Reloading texture '{0}'", tag);
                            wb3d::AssetManager::emplaceTexture(tag);
                            element->press_texture = tag;
                        }
                        else SHARD3D_WARN("File '{0}' does not exist!", tag);
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Transform2D")) {
                drawTransform2DControl("Position", element->position);
                drawTransform2DControl("Scale", element->scale, 0.1f);
                float rot = glm::degrees(element->rotation);
                ImGui::DragFloat("Rotation", &rot, 0.1f);
                element->rotation = glm::radians(rot);
                ImGui::DragInt("Depth", &element->zPos, 0.1f, 0, 127);

                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Anchoring")) {
                for (int y = 0; y < 3; y++)
                    for (int x = 0; x < 3; x++) {
                        if (x > 0)
                            ImGui::SameLine();
                        glm::vec2 coord = { static_cast<float>(x), static_cast<float>(y) };
                        if (ImGui::Button(std::string(std::to_string((coord.x / 2.f - .5f)) + ", " + std::to_string(coord.y / 2.f - .5f)).c_str(), ImVec2(50, 50))) {
                            element->anchorOffset = glm::vec2(coord.x / 2.f - 0.5f, coord.y / 2.f - 0.5f);
                        }
                    }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Script")) {
                bool exists = false;
                static char buffer[32];
                auto& name = element->scriptmodule;
                memset(buffer, 0, 32);
                strncpy(buffer, name.c_str(), 32);
                //const auto& actorClasses = DynamicScriptEngine::getActorClasses(actor.getComponent<Components::ScriptComponent>().lang);
                exists = DynamicScriptEngine::doesHUDClassExist("Shard3D.UI." + name, element->scriptlang);
                if (!exists) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.4, 0.7f, 1.0f));
                if (ImGui::InputText("Script Class", buffer, 32)) {
                    name = std::string(buffer);
                }
                if (!exists) ImGui::PopStyleColor();
                ImGui::Combo("Language", &element->scriptlang, "C#\0Visual Basic");
                ImGui::Checkbox("Is Button", &element->isActive);
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }
}
