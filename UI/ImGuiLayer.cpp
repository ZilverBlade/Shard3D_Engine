#include "ImGuiLayer.hpp"
#include "imgui_implementation.hpp"

#include <imgui.h>
#include <../imguizmo/ImGuizmo.h>

#include "imgui_glfw_implementation.hpp"
#include <stdexcept>
#include <iostream>
#include "..\engine_logger.hpp"
#include <miniaudio.h>

#include "../wb3d/levelmgr.hpp"
#include "../wb3d/master_manager.hpp"
#include "../texture.hpp"

#define GLFW_INCLUDE_VULKAN

#include "../utils/dialogs.h"
#include "../video/video_decode.hpp"
#include "../systems/material_system.hpp"
#include "../singleton.hpp"

namespace Shard3D {
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer() {}

    void ImGuiLayer::attach(VkRenderPass renderPass) {
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
	}

	void ImGuiLayer::detach() {
     // check if has been detatched already, otherwise when program closes, otherwise imgui will try to destroy a context that doesnt exist
        if (hasBeenDetached) return;
        vkDestroyDescriptorPool(Singleton::engineDevice.device(), descriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ax::NodeEditor::DestroyEditor(nodeEditorContext);
        levelTreePanel.destroyContext();
        levelPropertiesPanel.destroyContext();
        levelPeekPanel.destroyContext();
        hasBeenDetached = true;
	}

    void ImGuiLayer::update(VkCommandBuffer buffer, float dt) {
        if (hasBeenDetached) return;

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(ENGINE_SETTINGS_PATH);

        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = dt;

        glfwGetWindowSize(Singleton::engineWindow.getGLFWwindow(), &width, &height);
        io.DisplaySize = ImVec2(width, height);

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ax::NodeEditor::SetCurrentEditor(nodeEditorContext);
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

        ImGui::Begin("Viewport", (bool*)(true));
        ImGui::Image(Singleton::viewportImage, ImGui::GetWindowViewport()->Size);
        ImGui::End();
        // start rendering stuff here
        //ImGuizmo::BeginFrame();
        levelTreePanel.render();
        levelPropertiesPanel.render(levelTreePanel);
        levelPeekPanel.render();
        //levelGizmo.render(level, levelTreePanel);


        if (showTest) {
            ImGui::Begin("Material editor");
            ax::NodeEditor::Begin("Matedit BP");
            int uniqueId = 1;

            // Start drawing nodes.
            ax::NodeEditor::BeginNode(uniqueId++);
            ImGui::Text("Material");
            ax::NodeEditor::PinPivotSize(ImVec2(8, 8));
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            ImGui::Text("Diffuse");
            ax::NodeEditor::EndPin();

            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            ImGui::Text("Specular");
            ax::NodeEditor::EndPin();
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            ImGui::Text("Roughness");
            ax::NodeEditor::EndPin();
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            ImGui::Text("Metalcy");
            ax::NodeEditor::EndPin();
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            ImGui::Text("Reflectiveness");
            ax::NodeEditor::EndPin();

            ImGui::SameLine();
            ax::NodeEditor::EndNode();
            ax::NodeEditor::End();
            ImGui::End();
        }

        if (showStatsWindow) {
            ImGui::Begin("Stats");
            timeSinceLastSecond += dt;
            if (timeSinceLastSecond > 1.f) { deltaTimeFromLastSecond = dt; timeSinceLastSecond = 0; }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTimeFromLastSecond * 1000, 1 / deltaTimeFromLastSecond);
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
                ImGui::ColorPicker3("Default world background colour", enset.defaultBGColor, edpref.displayFloatOr255);
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
                if (ImGui::CollapsingHeader("Logging", ImGuiTreeNodeFlags_None)) {
                    ImGui::Checkbox("log.ModelLoadInfo", nullptr);
                }
                if (ImGui::CollapsingHeader("Warnings", ImGuiTreeNodeFlags_None)) {
                    ImGui::Checkbox("warn.NotInverseSquareAttenuation", nullptr);
                    ImGui::Checkbox("warn.InvertedSpotlightAngle ", nullptr);
                }
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

        ImGui::End();
        ImGui::Render();

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        glfwMakeContextCurrent(backup_current_context);
    }

    void ImGuiLayer::pushError(const char* message) {
        console.AddLog(message);
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
                        std::string filepath = FileDialogs::openFile(WORLDBUILDER3D_FILE_OPTIONS);
                        if (!filepath.empty()) {
                            levelTreePanel.clearSelectedActor();
                            Singleton::activeLevel->killEverything();
                            wb3d::MasterManager::loadLevel(filepath);
                        }
                    }
                }
                if (ImGui::MenuItem("Save Level...", "Ctrl+S")) {
                    std::string filepath = FileDialogs::saveFile(WORLDBUILDER3D_FILE_OPTIONS);
                    if (!filepath.empty()) {
                        wb3d::LevelManager levelMan(Singleton::activeLevel);
                        levelMan.save(filepath, false);
                    }
                }
                if (ImGui::MenuItem("Save Level... (Encrypted)", NULL)) {
                    std::string filepath = FileDialogs::saveFile(WORLDBUILDER3D_FILE_OPTIONS);
                    if (!filepath.empty()) {
                        wb3d::LevelManager levelMan(Singleton::activeLevel);
                        levelMan.save(filepath, true);
                    }
                }
                if (ImGui::MenuItem("Save Level As...", "Ctrl+Shift+S")) {
                    std::string filepath = FileDialogs::saveFile(WORLDBUILDER3D_FILE_OPTIONS);
                    if (!filepath.empty()) {
                        wb3d::LevelManager levelMan(Singleton::activeLevel);
                        levelMan.save(filepath, false);
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Close WorldBuilder3D", "Esc")) { detach(); return; }
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
                        std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: SIMULATING)";
                        glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                    } ImGui::EndDisabled();   
                    if (Singleton::activeLevel->simulationState != PlayState::Paused) {
                        ImGui::BeginDisabled(Singleton::activeLevel->simulationState != PlayState::Simulating); if (ImGui::MenuItem("Pause")) {
                            Singleton::activeLevel->simulationState = PlayState::Paused;
                        std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: Paused)";
                        glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                    } ImGui::EndDisabled();  }
                    else {
                        ImGui::BeginDisabled(Singleton::activeLevel->simulationState == PlayState::Simulating); if (ImGui::MenuItem("Resume")) {
                            Singleton::activeLevel->simulationState = PlayState::Simulating;
                        std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: SIMULATING)";
                        glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                    } ImGui::EndDisabled();
                    }
                    
                    ImGui::BeginDisabled(Singleton::activeLevel->simulationState == PlayState::Stopped);
                    if (ImGui::MenuItem("End")) {
                        levelTreePanel.clearSelectedActor();
                        Singleton::activeLevel->end();
                        levelTreePanel.setContext(Singleton::activeLevel);
                        levelPropertiesPanel.setContext(Singleton::activeLevel);
                        levelPeekPanel.setContext(Singleton::activeLevel);
                        std::string title = "Shard3D Engine " + ENGINE_VERSION + " (Playstate: Null)";
                        glfwSetWindowTitle(Singleton::engineWindow.getGLFWwindow(), title.c_str());
                    } ImGui::EndDisabled();
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Create Blueprint")) {}
                if (ImGui::MenuItem("Create Module Definition")) {}
                //if (ImGui::MenuItem("", NULL /*make sure to add some sort of shardcut */)) {}

                ImGui::EndMenu();
            }
#ifndef NDEBUG
            if (ImGui::BeginMenu("Debug")) {
                ImGui::TextDisabled("Shard3D Debug menu");
                ImGui::Separator();
                if (ImGui::MenuItem("Play test audio")) {
                }
                if (ImGui::MenuItem("Play test video")) {
                    VideoPlaybackEngine::EngineH264Video videoEngine;
                    videoEngine.createVideoSession(Singleton::engineWindow.getGLFWwindow(), "assets/mediadata/video.wmw");
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
                    std::string originalString = "Hello World! ABCDabcd0123<> /\\[]+=.;'`~����";
                    console.AddLog("Input string: %s", originalString.c_str());

                    char c;
                    std::string encryptedString;
                    for (int i = 0; i < originalString.length(); i++) {
                        c = originalString.at(i);
                        encryptedString.push_back((char)
                            ((((c + LEVEL_CIPHER_KEY) * 2) - LEVEL_CIPHER_KEY) / 2));
                    }

                    console.AddLog("Encrypted result: %s", encryptedString.c_str());
                    std::string decryptedString;

                    for (int i = 0; i < encryptedString.length(); i++) {
                        c = encryptedString.at(i);
                        decryptedString.push_back((char)
                            (((c * 2) + LEVEL_CIPHER_KEY) / 2) - LEVEL_CIPHER_KEY);
                    }

                    console.AddLog("Decrypted result: %s", decryptedString.c_str());

                    if (decryptedString != originalString) pushError("Encryption and decryption don't match! Are you using a cipher key that is a multiple of 2?");
                    else console.AddLog("Encryption and decryption match! Success!");
                }
        
                //if (ImGui::MenuItem("", NULL /*make sure to add some sort of shardcut */)) {}

                ImGui::EndMenu();
             }
#endif // DEBUG
     
            if (ImGui::BeginMenu("Actions")) {
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
                if (ImGui::MenuItem("Console", NULL /*make sure to add some sort of shardcut */)) {
                    //showTest = true;
                }
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

    }
}