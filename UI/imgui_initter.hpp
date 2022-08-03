#pragma once

#include <imgui.h>
#include <vulkan/vulkan.h>
namespace Shard3D {
	namespace wb3d {
		class ImGuiLayer;
	}
	class ImGuiInitter {
		_S3D_GVAR VkDescriptorPool imGuiDescriptorPool;
		friend class ImGuiLayer;
		friend class EditorApp;
		static void init(bool lightTheme);
	};
}