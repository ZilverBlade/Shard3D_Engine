#pragma once
#include "../s3dtpch.h"
#include <imgui.h>
#include "imgui_glfw_implementation.hpp"
#include "imgui_implementation.hpp"
#include "../singleton.hpp"
namespace Shard3D {
	class ImGuiInitter {
	private:
		friend class EditorApp;
		inline static VkDescriptorPool imGuiDescriptorPool;
		static void init() {
			//imgui hell

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
			vkCreateDescriptorPool(Globals::engineDevice.device(), &pool_info, nullptr, &imGuiDescriptorPool);

			Globals::imgui_init_info.DescriptorPool = imGuiDescriptorPool;
			Globals::imgui_init_info.Device = Globals::engineDevice.device();
			Globals::imgui_init_info.Queue = Globals::engineDevice.graphicsQueue();
			Globals::imgui_init_info.QueueFamily = Globals::engineDevice.findPhysicalQueueFamilies().graphicsFamily;

			Globals::imgui_init_info.MSAASamples = Globals::engineDevice.msaaSamples;
			ImGui_ImplGlfw_InitForVulkan(Globals::engineWindow.getGLFWwindow(), true);
			ImGui_ImplVulkan_Init(&Globals::imgui_init_info, Globals::engineRenderer.getSwapChainRenderPass());


			auto commandBuffer = Globals::engineDevice.beginSingleTimeCommands();
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
			Globals::engineDevice.endSingleTimeCommands(commandBuffer);
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

			style.AntiAliasedFill = true;
			style.AntiAliasedLines = true;

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
				
			Globals::viewportImage = ImGui_ImplVulkan_AddTexture(Globals::mainOffScreen.getSampler(), Globals::mainOffScreen.getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
#if ALLOW_PREVIEW_CAMERA
			Globals::previewViewportImage = ImGui_ImplVulkan_AddTexture(Globals::previewCamOffScreen.getSampler(), Globals::previewCamOffScreen.getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
#endif // !_DEPLOY
		}
	};
}