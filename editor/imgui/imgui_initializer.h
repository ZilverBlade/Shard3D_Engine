#pragma once

#include <vulkan/vulkan.h>
#include <Shard3D/core/vulkan_api/device.h>
#include "imgui_implementation.h"

namespace Shard3D {
	class ImGuiLayer;
	inline namespace Rendering {
		class FrameBufferAttachment;
	}
	class ImGuiInitializer {
		static inline VkDescriptorPool imGuiDescriptorPool;
		static inline ImGui_ImplVulkan_InitInfo imgui_init_info{};
		friend class ImGuiLayer;
		friend class Application;
		static void init(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, bool lightTheme);
		static void setViewportImage(VkDescriptorSet* image, FrameBufferAttachment* frameBufferAttachment);
	};
}