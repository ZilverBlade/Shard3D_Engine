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

#define GLFW_INCLUDE_VULKAN

#include "../utils/dialogs.h"
namespace Shard3D {


	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer() {}

    void ImGuiLayer::attach(VkRenderPass renderPass, EngineDevice* device, GLFWwindow* window, std::shared_ptr<wb3d::Level>& level) {
        currentDevice = device;
        glfwSetWindowTitle(window, "Shard3D Engine 1.0.8 (Playstate: Null)");
        hasBeenDetached = false;

        // Load any panels
        nodeEditorContext = ax::NodeEditor::CreateEditor();
        levelTreePanel.setContext(level);
        levelPropertiesPanel.setContext(level);
        //levelGizmo.setContext(level);

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        io.ConfigDockingWithShift = true;
        // temp
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        io.Fonts->AddFontFromFileTTF(DEFAULT_ENGINE_FONT, ENGINE_FONT_SIZE);
        io.Fonts->Build();

        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        vkCreateDescriptorPool(device->device(), &pool_info, nullptr, &descriptorPool);

        device->init_info.DescriptorPool = descriptorPool;
        device->init_info.Device = device->device();
        device->init_info.Queue = device->graphicsQueue();
        device->init_info.QueueFamily = device->findPhysicalQueueFamilies().graphicsFamily;

        device->init_info.MSAASamples = device->msaaSamples;
        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_Init(&device->init_info, renderPass);

        auto commandBuffer = device->beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        device->endSingleTimeCommands(commandBuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
        colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.36f, 0.16f, 0.96f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.01f, 0.01f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.01f, 0.01f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.90f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.42f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.17f, 0.17f, 0.17f, 0.35f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.32f, 0.32f, 0.32f, 0.59f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.36f, 0.16f, 0.96f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 0.77f);
        colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.67f, 0.60f, 0.29f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.36f, 0.16f, 0.96f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.46f, 0.24f, 1.00f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.69f, 0.30f, 1.00f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.00f, 0.96f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        style.DisabledAlpha = 0.3f;

        style.PopupRounding = 3;

        style.WindowPadding = ImVec2(4, 4);
        style.FramePadding = ImVec2(6, 4);
        style.ItemSpacing = ImVec2(6, 2);

        style.ScrollbarSize = 18;

        style.WindowBorderSize = 1;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 1;

        style.WindowRounding = 3;
        style.ChildRounding = 3;
        style.FrameRounding = 3;
        style.ScrollbarRounding = 2;
        style.GrabRounding = 3;

        style.AntiAliasedFill = edpref.antiAliasedUI;
        style.AntiAliasedLines = edpref.antiAliasedUI;

#ifdef IMGUI_HAS_DOCK 
        style.TabBorderSize = 1;
        style.TabRounding = 3;

        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
#endif

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
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ax::NodeEditor::DestroyEditor(nodeEditorContext);
        levelTreePanel.destroyContext();
        levelPropertiesPanel.destroyContext();

        hasBeenDetached = true;
	}

    void ImGuiLayer::update(VkCommandBuffer buffer, GLFWwindow* window, float dt, std::shared_ptr<wb3d::Level>& level) {
        if (hasBeenDetached) return;

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(ENGINE_SETTINGS_PATH);

        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = dt;

        glfwGetWindowSize(window, &width, &height);
        io.DisplaySize = ImVec2(width, height);

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // start rendering stuff here
        //ImGuizmo::BeginFrame();

        levelTreePanel.render();
        levelPropertiesPanel.render(levelTreePanel, currentDevice);
        //levelGizmo.render(level, levelTreePanel);

        ax::NodeEditor::SetCurrentEditor(nodeEditorContext);
        static bool visible = true;

        //ImGui::ShowDemoWindow(&visible);

#pragma region DOCKSPACE    
        static bool opt_fullscreen = false;
        static bool opt_padding = false;

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
        ImGui::Begin("Shard3D", &visible, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();
        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
       if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
          ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
          ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
           // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
       }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::TextDisabled("WorldBuilder3D 0.1");
                ImGui::Separator();
                ImGui::BeginDisabled(level->simulationState != PlayState::Stopped);
                if (ImGui::MenuItem("New Level", "Ctrl+N")) {
#ifdef _WIN32
                    if (MessageBoxA(glfwGetWin32Window(window), "This will destroy the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
                        
                        levelTreePanel.clearSelectedActor();
                        level->killEverything();
                    }
#endif
#ifdef __linux__ 
                    std::cout << "unsupported function\n";
#endif
                }
                if (ImGui::MenuItem("Load Level...", "Ctrl+O")) {
#ifdef _WIN32
                    if (MessageBoxA(glfwGetWin32Window(window), "This will overwrite the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
                        std::string filepath = FileDialogs::openFile(WORLDBUILDER3D_FILE_OPTIONS);
                        if (!filepath.empty()) {
                            levelTreePanel.clearSelectedActor();
                            level->killEverything();
                            wb3d::MasterManager::loadLevel(filepath);
                        }
                    }
#endif
#ifdef __linux__ 
                    std::cout << "unsupported function\n";
#endif
                }
                if (ImGui::MenuItem("Save Level...", "Ctrl+S")) {
#ifdef _WIN32
                    std::string filepath = FileDialogs::saveFile(WORLDBUILDER3D_FILE_OPTIONS);
                    if (!filepath.empty()) {
                        wb3d::LevelManager levelMan(level);
                        levelMan.save(filepath, false);
                    }
#endif
#ifdef __linux__ 
                    std::cout << "unsupported function\n";
#endif
                }
                if (ImGui::MenuItem("Save Level... (Encrypted)", NULL)) {
#ifdef _WIN32
                    std::string filepath = FileDialogs::saveFile(WORLDBUILDER3D_FILE_OPTIONS);
                    if (!filepath.empty()) {
                        wb3d::LevelManager levelMan(level);
                        levelMan.save(filepath, true);
                    }
#endif
#ifdef __linux__ 
                    std::cout << "unsupported function\n";
#endif
                }
                if (ImGui::MenuItem("Save Level As...", "Ctrl+Shift+S")) {
#ifdef _WIN32
                    std::string filepath = FileDialogs::saveFile(WORLDBUILDER3D_FILE_OPTIONS);
                    if (!filepath.empty()) {
                        wb3d::LevelManager levelMan(level);
                        levelMan.save(filepath, false);
                    }
#endif
#ifdef __linux__ 
                    std::cout << "unsupported function\n";
#endif
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
                    ImGui::BeginDisabled(level->simulationState != PlayState::Stopped || level->simulationState == PlayState::Paused);
                    if (ImGui::MenuItem("Begin")) {
                        wb3d::MasterManager::captureLevel(level);
                        level->begin();
                        glfwSetWindowTitle(window, "Shard3D Engine 1.0.8 (Playstate: SIMULATING)");      
                    } ImGui::EndDisabled();   
                    if (level->simulationState != PlayState::Paused) {
                        ImGui::BeginDisabled(level->simulationState != PlayState::Simulating); if (ImGui::MenuItem("Pause")) {
                        level->simulationState = PlayState::Paused;
                        glfwSetWindowTitle(window, "Shard3D Engine 1.0.8 (Playstate: Paused)");

                    } ImGui::EndDisabled();  }
                    else {
                        ImGui::BeginDisabled(level->simulationState == PlayState::Simulating); if (ImGui::MenuItem("Resume")) {
                        level->simulationState = PlayState::Simulating;
                        glfwSetWindowTitle(window, "Shard3D Engine 1.0.8 (Playstate: SIMULATING)");
                    } ImGui::EndDisabled();
                    }
                    
                    ImGui::BeginDisabled(level->simulationState == PlayState::Stopped);
                    if (ImGui::MenuItem("End")) {
                        levelTreePanel.clearSelectedActor();
                        level->end(); 
                        glfwSetWindowTitle(window, "Shard3D Engine 1.0.8 (Playstate: Null)");
                    } ImGui::EndDisabled();
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Add Actor Blueprint", NULL /*make sure to add some sort of shardcut */)) {}
                if (ImGui::MenuItem("Add Struct Definition", NULL /*make sure to add some sort of shardcut */)) {}
                //if (ImGui::MenuItem("", NULL /*make sure to add some sort of shardcut */)) {}

                ImGui::EndMenu();
            }
#ifndef NDEBUG
            if (ImGui::BeginMenu("Debug")) {
                ImGui::TextDisabled("Shard3D Debug menu");
                ImGui::Separator();
                if (ImGui::MenuItem("Play test audio", NULL /*make sure to add some sort of shardcut */)) {
                }
                if (ImGui::MenuItem("Encrypt string")) {
                    std::string originalString = "Hello World! ABCDabcd0123<> /\\[]+=.;'`~óòçñ";
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

            ImGui::EndMenuBar();
        }

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
                    Log log;
                    log.logString("Saved engine settings succesfully");
                }
                else {
                    std::cout << "Failed to write to ini file\n";
                }
            }
            if (ImGui::Button("Revert Changes")) {}
            ImGui::End();
        }

        console.Draw("Dev Console", &visible);

        ImGui::End();
#pragma endregion
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

}