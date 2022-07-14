#pragma once

#include <imgui.h>
#include <vulkan/vulkan.h>
namespace Shard3D {
	namespace wb3d {
		class ImGuiLayer;
	}
	class ImGuiInitter {
		inline static VkDescriptorPool imGuiDescriptorPool;
		friend class ImGuiLayer;
		friend class EditorApp;
		static void init();
	};
}