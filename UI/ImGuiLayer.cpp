#include "../s3dtpch.h" 
#include "ImGuiLayer.hpp"
#include "imgui_implementation.hpp"

#include <imgui.h>
#include <../imguizmo/ImGuizmo.h>

#include "imgui_glfw_implementation.hpp"

#include "..\engine_logger.hpp"
#include <miniaudio.h>

#include "../wb3d/levelmgr.hpp"
#include "../wb3d/master_manager.hpp"
#include "../texture.hpp"

#include "../audio.hpp"
#include "../utils/dialogs.h"
#include "../utils/stats_timing.h"
#ifndef NDEBUG
#include <video_decode.h>
#endif
#include "../systems/material_system.hpp"
#include "../singleton.hpp"
#include "imgui_initter.hpp"
#include <shellapi.h>
#include "../wb3d/assetmgr.hpp"
#include "HUDLayer.hpp"
#include <imgui_internal.h>


namespace Shard3D {
    ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

    ImGuiLayer::~ImGuiLayer() {}

    void ImGuiLayer::attach(VkRenderPass renderPass, LayerStack* layerStack) {
        currentStack = layerStack;
        std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: Null)";
        glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
        hasBeenDetached = false;

        // Load any panels
        nodeEditorContext = ax::NodeEditor::CreateEditor();
        levelTreePanel.setContext(Singleton::activeLevel);
        levelPropertiesPanel.setContext(Singleton::activeLevel);
        levelPeekPanel.setContext(Singleton::activeLevel);

        //levelGizmo.setContext(level);

        // set the default values for the structure from ini file so that you can actually modify them
        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(ENGINE_SETTINGS_PATH);

        enset.DEFAULT_WIDTH = ini.GetLongValue("WINDOW", "DEFAULT_WIDTH");
        enset.DEFAULT_HEIGHT = ini.GetLongValue("WINDOW", "DEFAULT_HEIGHT");
        enset.Resizable = ini.GetBoolValue("WINDOW", "Resizable");
        strncpy(enset.WindowName, ini.GetValue("WINDOW", "WindowName"), 64);

        if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") enset.ViewCombo = 0;
        else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") enset.ViewCombo = 1;
        enset.NearClipDistance = ini.GetDoubleValue("RENDERING", "NearClipDistance");
        enset.FarClipDistance = ini.GetDoubleValue("RENDERING", "FarClipDistance");
        enset.FOV = ini.GetDoubleValue("RENDERING", "FOV");
        enset.defaultBGColor[0] = ini.GetDoubleValue("RENDERING", "DefaultBGColorR");
        enset.defaultBGColor[1] = ini.GetDoubleValue("RENDERING", "DefaultBGColorG");
        enset.defaultBGColor[2] = ini.GetDoubleValue("RENDERING", "DefaultBGColorB");
        enset.pbr = ini.GetBoolValue("RENDERING", "PBR");

        createIcons();
    }

    void ImGuiLayer::createIcons() {
        {
            auto& img = _special_assets::_editor_icons.at("editor.play");
            icons.play = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.pause");
            icons.pause = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.stop");
            icons.stop = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.pref");
            icons.pref = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);       
        } {
            auto& img = _special_assets::_editor_icons.at("editor.save");
            icons.save = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.preview");
            icons.preview = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);       
        } {
            auto& img = _special_assets::_editor_icons.at("editor.layout");
            icons.layout = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.load");
            icons.load = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.level");
            icons.level = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.viewport");
            icons.viewport = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        } {
            auto& img = _special_assets::_editor_icons.at("editor.settings");
            icons.settings = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    void ImGuiLayer::detach() {
        // check if has been detatched already, otherwise when program closes, otherwise imgui will try to destroy a context that doesnt exist
        if (hasBeenDetached) return;
        vkDeviceWaitIdle(Singleton::engineDevice.device());
        vkDestroyDescriptorPool(Singleton::engineDevice.device(), ImGuiInitter::imGuiDescriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ax::NodeEditor::DestroyEditor(nodeEditorContext);
        levelTreePanel.destroyContext();
        levelPropertiesPanel.destroyContext();
        levelPeekPanel.destroyContext();
        hasBeenDetached = true;
        SHARD3D_LOG("Detached ImGui");
    }

    void ImGuiLayer::update(FrameInfo& frameInfo) {
        if (hasBeenDetached) return;
        if (hasBeenDetached) SHARD3D_ERROR("FAILED TO DETACH IMGUI");
        SHARD3D_STAT_RECORD();
        if (refreshContext) {
            levelTreePanel.setContext(Singleton::activeLevel);
            levelPropertiesPanel.setContext(Singleton::activeLevel);
            levelPeekPanel.setContext(Singleton::activeLevel);
            refreshContext = false;
        }
        
        SHARD3D_EVENT_BIND_HANDLER(ImGuiLayer::eventEvent);

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(ENGINE_SETTINGS_PATH);

        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = frameInfo.frameTime;

        glfwGetWindowSize(Singleton::engineWindow.getGLFWwindow(), &width, &height);
        io.DisplaySize = ImVec2(width, height);

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ax::NodeEditor::SetCurrentEditor(nodeEditorContext);
        static bool visible = true;

        //ImGui::ShowDemoWindow(&visible);
        
#pragma region boilerplate dockspace code    
        static bool opt_fullscreen = true;
        static bool opt_padding = true;

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else { dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode; }
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("MyDockSpace", &visible, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();
        if (opt_fullscreen)
            ImGui::PopStyleVar(2);
#pragma endregion
        // Submit the DockSpace
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar()) {
            renderMenuBar();
            ImGui::EndMenuBar();
        }
        
        renderQuickBar();

        // VIEWPORT
        ImGui::Begin("Viewport");
            ImVec2 vSize = ImGui::GetContentRegionAvail();
            tempInfo::aspectRatioWoH = vSize.x / vSize.y;
        ImGui::Image(Singleton::viewportImage, vSize);
        if (Singleton::activeLevel->simulationState == PlayState::Paused) { 
            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            vMin.x += ImGui::GetWindowPos().x + 8;
            vMin.y += ImGui::GetWindowPos().y + 16;
            ImGui::GetWindowDrawList()->AddImage(icons.pause, vMin, ImVec2(vMin.x + 64, vMin.y + 64), ImVec2(0, 0), ImVec2(1, 1)); 
        }
        if (Singleton::editorPreviewSettings.ONLY_GAME) {
           ImVec2 vMin = ImGui::GetWindowContentRegionMin();
           vMin.x += ImGui::GetWindowPos().x + 80;
           vMin.y += ImGui::GetWindowPos().y + 16;
         // ImGui::GetWindowDrawList()->AddText(io.Fonts->Fonts[0], 14.f, ImVec2(vMin.x, vMin.y), ImU32(1.f), "Test");
           ImGui::GetWindowDrawList()->AddImage(icons.preview, vMin, ImVec2(vMin.x + 64, vMin.y + 64), ImVec2(0, 0), ImVec2(1, 1));
       }
        ImGui::End();
        // start rendering stuff here
        //ImGuizmo::BeginFrame();
        hudBuilder.render();
        levelTreePanel.render();
        levelPropertiesPanel.render(levelTreePanel);
        levelPeekPanel.render();
        AssetExplorerPanel.render();
       
        //levelGizmo.render(level, levelTreePanel);


        if (showTest) {
            ImGui::Begin("Material editor", &showTest);
            ImGui::DragFloat("Specular", &Singleton::testPBR.x, 0.01f, 0.f, 2.f);
            ImGui::DragFloat("Roughness", &Singleton::testPBR.y, 0.01f, 0.f, 1.f);
            ImGui::BeginDisabled(!enset.pbr);
            ImGui::DragFloat("Metallic", &Singleton::testPBR.z, 0.01f, 0.f, 1.f);
            ImGui::EndDisabled();
            //ax::NodeEditor::Begin("Matedit BP");
            //int uniqueId = 1;
            //
            //// Start drawing nodes.
            //ax::NodeEditor::BeginNode(uniqueId++);
            //ImGui::Text("Material");
            //ax::NodeEditor::PinPivotSize(ImVec2(8, 8));
            //ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            //ImGui::Text("Diffuse");
            //ax::NodeEditor::EndPin();
            //
            //ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            //ImGui::Text("Specular");
            //ax::NodeEditor::EndPin();
            //ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            //ImGui::Text("Roughness");
            //ax::NodeEditor::EndPin();
            //ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            //ImGui::Text("Metalcy");
            //ax::NodeEditor::EndPin();
            //ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            //ImGui::Text("Reflectiveness");
            //ax::NodeEditor::EndPin();
            //
            //ImGui::SameLine();
            //ax::NodeEditor::EndNode();
            //ax::NodeEditor::End();
            ImGui::End();
        }
        if (showCredits) {
            ImGui::Begin("Credits", &showCredits);
            if (ImGui::TreeNodeEx("Programming", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("ZilverBlade (Lead)");
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Graphics", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("ZilverBlade (Lead)");
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Editor Design", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("ZilverBlade (Layout and Colours)");
                ImGui::Text("Dhibaid (Icons)");
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Special thanks", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Brendan Galea (Vulkan tutorial series)");
                ImGui::Text("The Cherno (Game engine series)");
                ImGui::Text("All of the creators of the used libraries");
                ImGui::TreePop();
            }
            ImGui::End();
        }

        if (showStatsWindow) {
            ImGui::Begin("Stats");
            timeSinceLastSecond += frameInfo.frameTime;
            if (timeSinceLastSecond > 1.f) { deltaTimeFromLastSecond = frameInfo.frameTime; timeSinceLastSecond = 0; }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTimeFromLastSecond * 1000.f, 1.f / deltaTimeFromLastSecond);
            ImGui::End();
        }
        if (showStylizersWindow) {
            ImGui::Begin("Stats");
            timeSinceLastSecond += frameInfo.frameTime;
            if (timeSinceLastSecond > 1.f) { deltaTimeFromLastSecond = frameInfo.frameTime; timeSinceLastSecond = 0; }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTimeFromLastSecond * 1000.f, 1.f / deltaTimeFromLastSecond);
            if (ImGui::CollapsingHeader("style editor")) {
                ImGui::ShowStyleEditor();
            }
            ImGui::End();
        }
        if (showEngineSettingsWindow) {
            ImGui::Begin("Engine Settings", &showEngineSettingsWindow);
            if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_None)) {
                ImGui::InputInt("Default width", &enset.DEFAULT_WIDTH, 5, 50);
                ImGui::InputInt("Default height", &enset.DEFAULT_HEIGHT, 5, 50);
                ImGui::Checkbox("Resizable", &enset.Resizable);
                ImGui::InputText("Window name", enset.WindowName, IM_ARRAYSIZE(enset.WindowName));
            }
            if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_None)) {
                ImGui::Combo("View", &enset.ViewCombo, "Perspective\0Orthographic");

                ImGui::SliderFloat("Near clip distance", &enset.NearClipDistance, 0.000000001, 2);
                ImGui::SliderFloat("Far clip distance", &enset.FarClipDistance, 16, 32767);
                ImGui::SliderFloat("FOV", &enset.FOV, 10, 180);
                ImGui::ColorPicker3("Background colour", enset.defaultBGColor, edpref.displayFloatOr255);
                ImGui::Checkbox("Advanced materials (PBR)", &enset.pbr);
            }
            /*
            if (ImGui::CollapsingHeader("Engine Limits", ImGuiTreeNodeFlags_None)) {
                ImGui::SliderInt("MaxPointlights", nullptr, 1, 50);
                ImGui::SliderInt("MaxSpotlights", nullptr, 1, 50);
                ImGui::SliderInt("MaxDirectionalLights", nullptr, 1, 10);
                ImGui::SliderInt("MaxFramesInFlight", nullptr, 1, 16);
            }
            */
            if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_None)) {
               // if (ImGui::CollapsingHeader("Logging", ImGuiTreeNodeFlags_None)) {
               //     ImGui::Checkbox("log.ModelLoadInfo", nullptr);
               // }
               // if (ImGui::CollapsingHeader("Warnings", ImGuiTreeNodeFlags_None)) {
               //     ImGui::Checkbox("warn.NotInverseSquareAttenuation", nullptr);
               //     ImGui::Checkbox("warn.InvertedSpotlightAngle ", nullptr);
               // }
            }

            if (ImGui::Button("Save Changes")) {
                if (ini.GetSection("DEFAULT") != nullptr) {
                    ini.SetLongValue("WINDOW", "DEFAULT_WIDTH", enset.DEFAULT_WIDTH);
                    ini.SetLongValue("WINDOW", "DEFAULT_HEIGHT", enset.DEFAULT_HEIGHT);
                    ini.SetBoolValue("WINDOW", "Resizable", enset.defaultBGColor[2]);
                    ini.SetValue("WINDOW", "WindowName", enset.WindowName);

                    if (enset.ViewCombo == 0) ini.SetValue("RENDERING", "View", "Perspective");
                    else if (enset.ViewCombo == 1) ini.SetValue("RENDERING", "View", "Orthographic");
                    ini.SetDoubleValue("RENDERING", "NearClipDistance", enset.NearClipDistance);
                    ini.SetDoubleValue("RENDERING", "FarClipDistance", enset.FarClipDistance);
                    ini.SetDoubleValue("RENDERING", "FOV", enset.FOV);
                    ini.SetDoubleValue("RENDERING", "DefaultBGColorR", enset.defaultBGColor[0]);
                    ini.SetDoubleValue("RENDERING", "DefaultBGColorG", enset.defaultBGColor[1]);
                    ini.SetDoubleValue("RENDERING", "DefaultBGColorB", enset.defaultBGColor[2]);
                    ini.SetBoolValue("RENDERING", "PBR", enset.pbr);

                    ini.SaveFile(ENGINE_SETTINGS_PATH);
                    SHARD3D_INFO("Saved engine settings succesfully");
                }
                else {
                    SHARD3D_ERROR("Failed to write to ini file");
                }
            }
            if (ImGui::Button("Revert Changes")) {}
            ImGui::End();
        }
        if (showGraphicsSettingsWindow) {
            ImGui::Begin("Graphics Settings", &showGraphicsSettingsWindow);
            if (ImGui::CollapsingHeader("settings", ImGuiTreeNodeFlags_None)) {
               if(ImGui::Checkbox("V-Sync", &GraphicsSettings::get().VSync)) ;
                ImGui::DragInt("Antisotropic Filtering", &GraphicsSettings::get().maxAnisotropy, 0.08f, 1, 16);
               //ImGui::Combo("Window name", (int*)GraphicsSettings::get().LODCoef, "Poor\0Low\0Medium\0High\0\0Cinematic");
            }
            //if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_None)) {
            //    ImGui::Combo("View", &enset.ViewCombo, "Perspective\0Orthographic");
            //
            //    ImGui::SliderFloat("Near clip distance", &enset.NearClipDistance, 0.003f, 2.f);
            //    ImGui::SliderFloat("Far clip distance", &enset.FarClipDistance, 16.f, 32767.f);
            //    ImGui::SliderFloat("FOV", &enset.FOV, 10, 180);
            //    ImGui::ColorPicker3("Background colour", enset.defaultBGColor, edpref.displayFloatOr255);
            //    ImGui::Checkbox("Advanced materials (PBR)", &enset.pbr);
            //}
            /*
            if (ImGui::CollapsingHeader("Engine Limits", ImGuiTreeNodeFlags_None)) {
                ImGui::SliderInt("MaxPointlights", nullptr, 1, 50);
                ImGui::SliderInt("MaxSpotlights", nullptr, 1, 50);
                ImGui::SliderInt("MaxDirectionalLights", nullptr, 1, 10);
                ImGui::SliderInt("MaxFramesInFlight", nullptr, 1, 16);
            }
            */
           // if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_None)) {
                // if (ImGui::CollapsingHeader("Logging", ImGuiTreeNodeFlags_None)) {
                //     ImGui::Checkbox("log.ModelLoadInfo", nullptr);
                // }
                // if (ImGui::CollapsingHeader("Warnings", ImGuiTreeNodeFlags_None)) {
                //     ImGui::Checkbox("warn.NotInverseSquareAttenuation", nullptr);
                //     ImGui::Checkbox("warn.InvertedSpotlightAngle ", nullptr);
                // }
            //}

           // if (ImGui::Button("Save Changes")) {
           //     if (ini.GetSection("DEFAULT") != nullptr) {
           //         ini.SetLongValue("WINDOW", "DEFAULT_WIDTH", enset.DEFAULT_WIDTH);
           //         ini.SetLongValue("WINDOW", "DEFAULT_HEIGHT", enset.DEFAULT_HEIGHT);
           //         ini.SetBoolValue("WINDOW", "Resizable", enset.defaultBGColor[2]);
           //         ini.SetValue("WINDOW", "WindowName", enset.WindowName);
           //
           //         if (enset.ViewCombo == 0) ini.SetValue("RENDERING", "View", "Perspective");
           //         else if (enset.ViewCombo == 1) ini.SetValue("RENDERING", "View", "Orthographic");
           //         ini.SetDoubleValue("RENDERING", "NearClipDistance", enset.NearClipDistance);
           //         ini.SetDoubleValue("RENDERING", "FarClipDistance", enset.FarClipDistance);
           //         ini.SetDoubleValue("RENDERING", "FOV", enset.FOV);
           //         ini.SetDoubleValue("RENDERING", "DefaultBGColorR", enset.defaultBGColor[0]);
           //         ini.SetDoubleValue("RENDERING", "DefaultBGColorG", enset.defaultBGColor[1]);
           //         ini.SetDoubleValue("RENDERING", "DefaultBGColorB", enset.defaultBGColor[2]);
           //         ini.SetBoolValue("RENDERING", "PBR", enset.pbr);
           //
           //         ini.SaveFile(ENGINE_SETTINGS_PATH);
           //         SHARD3D_INFO("Saved engine settings succesfully");
           //     }
           //     else {
           //         SHARD3D_ERROR("Failed to write to ini file");
           //     }
           // }
           // if (ImGui::Button("Revert Changes")) {}
            ImGui::End();
        }
        if (showDemoWindow) ImGui::ShowDemoWindow();
        ImGui::End();
        ImGui::Render();

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frameInfo.commandBuffer);
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SHARD3D_STAT_RECORD_END({ "ImGui", "Rendering" });
    }

    void ImGuiLayer::attachGUIEditorInfo(std::shared_ptr<HUDContainer>& container) {
        hudBuilder.setContext(container);
    }

   
    void ImGuiLayer::renderQuickBar() {

        ImGui::Begin("_editor_quickbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
        ImVec2 btnSize = { 48.f, 48.f };
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = std::max((int)(panelWidth / (btnSize.x + 16.f))// <--- thumbnail size (96px) + padding (16px)
            , 1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
        ImGui::Columns(columnCount, 0, false);                
        // begin

        // Save/Load
        if (ImGui::ImageButton(icons.save, btnSize)) {
            SHARD3D_NOIMPL;
        }
        ImGui::TextWrapped("Save");
        ImGui::NextColumn();
        if (ImGui::ImageButton(icons.load, btnSize)) {
            SHARD3D_NOIMPL;
        }
        ImGui::TextWrapped("Load");
        ImGui::NextColumn();
        ImGui::NextColumn();

        // PLAY MODE
        if (ImGui::ImageButton(
            (Singleton::activeLevel->simulationState == PlayState::Stopped) ? icons.play : 
                ((Singleton::activeLevel->simulationState == PlayState::Paused) ? icons.play : icons.pause), btnSize))
            if (Singleton::activeLevel->simulationState == PlayState::Stopped) { // Play
                wb3d::MasterManager::captureLevel(Singleton::activeLevel);
                Singleton::activeLevel->begin();
                std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: SIMULATING) | " + Singleton::activeLevel->name;
                glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
            } else if (Singleton::activeLevel->simulationState == PlayState::Paused) {  // Resume
                Singleton::activeLevel->simulationState = PlayState::Simulating;
                Singleton::activeLevel->simulationStateCallback();
                std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: SIMULATING) | " + Singleton::activeLevel->name;
                glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());          

            } else if (Singleton::activeLevel->simulationState == PlayState::Simulating) {  // Pause
                Singleton::activeLevel->simulationState = PlayState::Paused;
                Singleton::activeLevel->simulationStateCallback();
                std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: Paused) | " + Singleton::activeLevel->name;
                glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());                           
            }
        ImGui::TextWrapped((Singleton::activeLevel->simulationState == PlayState::Stopped) ? "Play" :
            ((Singleton::activeLevel->simulationState == PlayState::Paused) ? "Resume" : "Pause"));
        ImGui::NextColumn();
        ImGui::BeginDisabled(Singleton::activeLevel->simulationState == PlayState::Stopped);
        if (ImGui::ImageButton(icons.stop, btnSize)) {        // Stop
            levelTreePanel.clearSelectedActor();
            Singleton::activeLevel->end();
            std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: Null) | " + Singleton::activeLevel->name;
            glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
            refreshContext = true;
        }
        ImGui::TextWrapped("Stop");
        ImGui::EndDisabled();

        ImGui::NextColumn();
        ImGui::NextColumn();
        // Editor settings
        if (ImGui::ImageButton(icons.pref, btnSize)) {
            SHARD3D_NOIMPL;
        }
        ImGui::TextWrapped("Pref");
        ImGui::NextColumn();
        // Editor settings
        if (ImGui::ImageButton(icons.settings, btnSize)) {
            SHARD3D_NOIMPL;
        }
        ImGui::TextWrapped("Config");
        ImGui::NextColumn();
        if (ImGui::ImageButton(icons.layout, btnSize)) {
            SHARD3D_NOIMPL;
        }
        ImGui::TextWrapped("Layout");
        ImGui::NextColumn();
        if (ImGui::ImageButton(icons.viewport, btnSize)) {
            SHARD3D_NOIMPL;
        }
        ImGui::TextWrapped("View");
        ImGui::NextColumn();
        // end
        ImGui::Columns(1);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::End();
    }

    void ImGuiLayer::renderMenuBar() {
        if (ImGui::BeginMenu("File")) {
            ImGui::TextDisabled("WorldBuilder3D 0.1");
            ImGui::Separator();
            ImGui::BeginDisabled(Singleton::activeLevel->simulationState != PlayState::Stopped);
            if (ImGui::MenuItem("New Level", "Ctrl+N")) {
                if (MessageDialogs::show("This will destroy the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MessageDialogs::OPTYESNO | MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTDEFBUTTON2) == MessageDialogs::RESYES) {
                    levelTreePanel.clearSelectedActor();
                    Singleton::activeLevel->killEverything();
                }

            }
            if (ImGui::MenuItem("Load Level...", "Ctrl+O")) {
                if (MessageDialogs::show("This will overwrite the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MessageDialogs::OPTYESNO | MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTDEFBUTTON2) == MessageDialogs::RESYES) {
                    std::string filepath = FileDialogs::openFile(ENGINE_WORLDBUILDER3D_LEVELFILE_OPTIONS);
                    if (!filepath.empty()) {
                        levelTreePanel.clearSelectedActor();
                        Singleton::activeLevel->killEverything();
                        wb3d::MasterManager::loadLevel(filepath);
                    }
                }
            }
            if (ImGui::MenuItem("Save Level...", "Ctrl+S")) {
                wb3d::LevelManager levelMan(Singleton::activeLevel);
                levelMan.save(Singleton::activeLevel->currentpath, false);
            }
            if (ImGui::MenuItem("Save Level... (Encrypted)", NULL)) {
                wb3d::LevelManager levelMan(Singleton::activeLevel);
                levelMan.save(Singleton::activeLevel->currentpath, true);
            }
            if (ImGui::MenuItem("Save Level As...", "Ctrl+Shift+S")) {
                std::string filepath = FileDialogs::saveFile(ENGINE_WORLDBUILDER3D_LEVELFILE_OPTIONS);
                if (!filepath.empty()) {
                    wb3d::LevelManager levelMan(Singleton::activeLevel);
                    levelMan.save(filepath, false);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close WorldBuilder3D", "Esc")) { currentStack->popOverlay(this); }
            ImGui::Separator();
            ImGui::EndDisabled();

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::TextDisabled("WorldBuilder3D 0.1");
            ImGui::Separator();
            if (ImGui::BeginMenu("Level Simulation")) {
                ImGui::BeginDisabled(Singleton::activeLevel->simulationState != PlayState::Stopped || Singleton::activeLevel->simulationState == PlayState::Paused);
                if (ImGui::MenuItem("Begin")) {
                    wb3d::MasterManager::captureLevel(Singleton::activeLevel);
                    Singleton::activeLevel->begin();
                    std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: SIMULATING) | " + Singleton::activeLevel->name;
                    glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                } ImGui::EndDisabled();
                if (Singleton::activeLevel->simulationState != PlayState::Paused) {
                    ImGui::BeginDisabled(Singleton::activeLevel->simulationState != PlayState::Simulating); if (ImGui::MenuItem("Pause")) {
                        Singleton::activeLevel->simulationState = PlayState::Paused;
                        Singleton::activeLevel->simulationStateCallback();
                        std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: Paused) | " + Singleton::activeLevel->name;
                        glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                    } ImGui::EndDisabled();
                }
                else {
                    ImGui::BeginDisabled(Singleton::activeLevel->simulationState == PlayState::Simulating); if (ImGui::MenuItem("Resume")) {
                        Singleton::activeLevel->simulationState = PlayState::Simulating;
                        Singleton::activeLevel->simulationStateCallback();
                        std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: SIMULATING) | " + Singleton::activeLevel->name;
                        glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                    } ImGui::EndDisabled();
                }

                ImGui::BeginDisabled(Singleton::activeLevel->simulationState == PlayState::Stopped);
                if (ImGui::MenuItem("End")) {
                    levelTreePanel.clearSelectedActor();
                    Singleton::activeLevel->end();
                    std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: Null) | " + Singleton::activeLevel->name;
                    glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                    refreshContext = true;
                } ImGui::EndDisabled();
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Create Blueprint")) {}
            if (ImGui::MenuItem("Create Module Definition")) {}
            //if (ImGui::MenuItem("", NULL /*make sure to add some sort of shardcut */)) {}

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
#ifndef _DEPLOY
            if (ImGui::BeginMenu("Rendering")) {
                ImGui::Checkbox("Preview Game", &Singleton::editorPreviewSettings.ONLY_GAME);
                ImGui::Separator();
                ImGui::Checkbox("Grid", &Singleton::editorPreviewSettings.V_GRID);
                ImGui::Checkbox("Billboards", &Singleton::editorPreviewSettings.V_EDITOR_BILLBOARDS);
                ImGui::Checkbox("HUD", &Singleton::editorPreviewSettings.V_GUI);
                ImGui::EndMenu();
            }
#endif
            ImGui::EndMenu();
        }
#ifndef NDEBUG
        if (ImGui::BeginMenu("Debug")) {
            ImGui::TextDisabled("Shard3D Debug menu");
            ImGui::Checkbox("Stylizer", &showStylizersWindow);
            ImGui::Checkbox("ImGuiDemo", &showDemoWindow);
            if (ImGui::MenuItem("Play test audio")) {
                EngineAudio audio;
                audio.play("assets/audiodata/thou-3.mp3");
            }

            if (ImGui::MenuItem("Play test video")) {
#ifndef NDEBUG
                VideoPlaybackEngine::EngineH264Video videoEngine;
                videoEngine.createVideoSession(Singleton::engineWindow.getGLFWwindow(), "assets/mediadata/video.wmw");
                while (videoEngine.isPlaying()) {
                    glfwPollEvents();
                }
#elif NDEBUG
                SHARD3D_NOIMPL;
#endif
            }
            if (ImGui::MenuItem("Save test material")) {
                MaterialSystem::Material surfaceMat;
                surfaceMat.type = SurfaceMaterial;
                surfaceMat.surfaceMaterial.surfaceProp = SurfaceStandardLit;
                surfaceMat.surfaceMaterial.surfaceMat = SurfaceOpaqueMaterial;
                surfaceMat.surfaceMaterial.diffuseColor = { 1.f, 0.f, 1.f, 1.f };
                surfaceMat.surfaceMaterial.roughnessTex.path = "assets/texturedata/coolroughness.png";

                MaterialSystem::saveMaterial(surfaceMat, "assets/materialdata/mycoolmat");
            }
            if (ImGui::MenuItem("Save test material list")) {
                MaterialSystem::Material surfaceMat;
                surfaceMat.type = SurfaceMaterial;
                surfaceMat.surfaceMaterial.surfaceProp = SurfaceStandardLit;
                surfaceMat.surfaceMaterial.surfaceMat = SurfaceOpaqueMaterial;
                surfaceMat.surfaceMaterial.diffuseColor = { 1.f, 0.f, 1.f, 1.f };
                surfaceMat.surfaceMaterial.roughnessTex.path = "assets/texturedata/coolroughness.png";
                MaterialSystem::saveMaterial(surfaceMat, "assets/materialdata/mycoolmat");
                MaterialSystem::Material surfaceMat2;
                surfaceMat2.type = SurfaceMaterial;
                surfaceMat2.surfaceMaterial.surfaceProp = SurfaceStandardUnlit;
                surfaceMat2.surfaceMaterial.surfaceMat = SurfaceMaskedMaterial;
                surfaceMat2.surfaceMaterial.diffuseTex.path = "assets/texturedata/grid.png";
                surfaceMat2.surfaceMaterial.maskTex.path = "assets/texturedata/gridmask.png";
                MaterialSystem::saveMaterial(surfaceMat2, "assets/materialdata/mycoolgridmat2");

                MaterialSystem::MaterialList matlist;
                matlist.list.push_back(surfaceMat);
                matlist.list.push_back(surfaceMat2);
                MaterialSystem::saveList(matlist, "assets/material-listdata/mycoollist");
            }
            if (ImGui::MenuItem("Encrypt string")) {
                std::string originalString = "Hello World! ABCDabcd0123<> /\\[]+=.;'`~óòçñ";
                char c;
                std::string encryptedString;
                for (int i = 0; i < originalString.length(); i++) {
                    c = originalString.at(i);
                    encryptedString.push_back((char)
                        ((((c + ENSET_WB3DLEVEL_CIPHER_KEY) * 2) - ENSET_WB3DLEVEL_CIPHER_KEY) / 2));
                }
                std::string decryptedString;

                for (int i = 0; i < encryptedString.length(); i++) {
                    c = encryptedString.at(i);
                    decryptedString.push_back((char)
                        (((c * 2) + ENSET_WB3DLEVEL_CIPHER_KEY) / 2) - ENSET_WB3DLEVEL_CIPHER_KEY);
                }

                if (decryptedString != originalString) SHARD3D_WARN("[WB3D] Encryption and decryption don't match! Are you using a cipher key that is a multiple of 2?");
                else SHARD3D_INFO("[WB3D] Encryption and decryption match! Success!");
            }

            //if (ImGui::MenuItem("", NULL /*make sure to add some sort of shardcut */)) {}

            ImGui::EndMenu();
        }
#endif
        if (ImGui::BeginMenu("Actions")) {
            if (ImGui::MenuItem("Dump current frame's stats", NULL /*make sure to add some sort of shardcut */)) SHARD3D_STAT_DUMP_ALL();
            if (ImGui::MenuItem("Compile Shaders", NULL /*make sure to add some sort of shardcut */)) {
                ShellExecuteA(nullptr, "open", "shadercompmgr.exe", "-o shaders/ shaders/", "/", false);
            }
            if (ImGui::MenuItem("Compile Shaders & Reupload Pipeline", NULL /*make sure to add some sort of shardcut */)) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.

            ImGui::TextDisabled("WorldBuilder3D 0.1");
            ImGui::Separator();
            if (ImGui::MenuItem("Engine Settings", NULL /*make sure to add some sort of shardcut */)) { showEngineSettingsWindow = true; }
            if (ImGui::MenuItem("WorldBuilder3D Settings", NULL /*make sure to add some sort of shardcut */)) { /*show editor win*/ }
            ImGui::Separator();
            if (ImGui::MenuItem("Game Graphics Settings", NULL /*make sure to add some sort of shardcut */)) { showGraphicsSettingsWindow = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("Material Builder", NULL /*make sure to add some sort of shardcut */)) {
                showTest = true;
            }
            ImGui::Separator();
            ImGui::Checkbox("Stats", &showStatsWindow);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
#ifdef WIN32
            if (ImGui::MenuItem("Main Website")) { ShellExecuteA(nullptr, "open", "https://www.shard3d.com", nullptr, nullptr, false); }
            if (ImGui::MenuItem("Documentation")) { ShellExecuteA(nullptr, "open", "https://docs.shard3d.com", nullptr, nullptr, false); }
            if (ImGui::MenuItem("WorldBuilder3D")) { ShellExecuteA(nullptr, "open", "https://docs.shard3d.com/worldbuilder3d.html", nullptr, nullptr, false); }
#endif  
#ifdef __linux__ 
            ImGui::MenuItem("Unsupported");
#endif
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Credits")) showCredits = true;
    }


    void ImGuiLayer::eventEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<MouseButtonDownEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseButtonDownEvent));
        dispatcher.dispatch<MouseButtonReleaseEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseButtonReleaseEvent));
        dispatcher.dispatch<MouseHoverEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseHoverEvent));
        dispatcher.dispatch<MouseScrollEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseScrollEvent));
        dispatcher.dispatch<KeyDownEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::keyDownEvent));
        dispatcher.dispatch<KeyReleaseEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::keyReleaseEvent));
        dispatcher.dispatch<KeyPressEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::keyPressEvent));
    }

    bool ImGuiLayer::mouseButtonDownEvent(MouseButtonDownEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[e.getButtonCode()] = true;

        return false;
    }

    bool ImGuiLayer::mouseButtonReleaseEvent(MouseButtonReleaseEvent& e)  {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[e.getButtonCode()] = false;

        return false;
    }

    bool ImGuiLayer::mouseHoverEvent(MouseHoverEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(e.getXPos(), e.getYPos());

        return false;
    }

    bool ImGuiLayer::mouseScrollEvent(MouseScrollEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel += e.getYOffset();
        io.MouseWheelH += e.getXOffset();

        return false;
    }
    
    bool ImGuiLayer::keyDownEvent(KeyDownEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[e.getKeyCode()] = true;
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
        return false;
    }

    bool ImGuiLayer::keyReleaseEvent(KeyReleaseEvent& e)  {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[e.getKeyCode()] = false;
        return false;
    }

    bool ImGuiLayer::keyPressEvent(KeyPressEvent& e)  {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter((unsigned short)e.getKeyCode());
        return false;
    }




}