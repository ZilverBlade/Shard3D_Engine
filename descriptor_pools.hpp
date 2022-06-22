#pragma once
#include "descriptors.hpp"

namespace Shard3D {

	class SharedPools{
	public: 
// (0 = global, 1 = per-material static, 2 = per-material dynamic, 3 = per-draw), 
// https://www.reddit.com/r/gamedev/comments/kybh22/vulkan_material_system_descriptor_management_and/

		inline static std::unique_ptr<EngineDescriptorPool> globalPool{};
		inline static std::unique_ptr<EngineDescriptorPool> staticMaterialPool{};
		inline static std::unique_ptr<EngineDescriptorPool> dynamicMaterialPool{};
		inline static std::unique_ptr<EngineDescriptorPool> drawPool{};

		inline static void destructPools() {
			globalPool = nullptr;
			staticMaterialPool = nullptr;
			dynamicMaterialPool = nullptr;
			drawPool = nullptr;
		}
	};
}