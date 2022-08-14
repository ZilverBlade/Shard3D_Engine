#pragma once

#include <imgui.h>
#include <vulkan/vulkan.h>
#include "imgui_implementation.h"
#include "../core/rendering/offscreen.h"

namespace Shard3D {
	namespace ECS {
		class ImGuiLayer;
	}
	class ImGuiInitter {
		static inline VkDescriptorPool imGuiDescriptorPool;
		static inline ImGui_ImplVulkan_InitInfo imgui_init_info{};
		friend class ImGuiLayer;
		friend class EngineApplication;
		static void init(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass,  bool lightTheme);
	};
}