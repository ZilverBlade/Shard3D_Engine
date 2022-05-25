#include "ImGuiLayer.hpp"
#include "imgui_implementation.hpp"
#include <imgui.h>

namespace Shard3D {

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer() {

	}

	void ImGuiLayer::attach(VkRenderPass renderPass, GLFWwindow* window) {
        

		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

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

        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        /*
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = VkInstanceCreateFlagBits;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily = g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = g_DescriptorPool;

        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = wd->ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        

        ImGui_ImplVulkan_Init(&init_info, renderPass);
        */
	}

	void ImGuiLayer::detach() {

	}

	void ImGuiLayer::update(VkCommandBuffer buffer, GLFWwindow* window, float dt) {

        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = dt;

        glfwGetWindowSize(window, &width, &height);
        io.DisplaySize = ImVec2(width, height);

        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();

        static bool visible = true;
        ImGui::ShowDemoWindow(&visible);

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
	}

}