#pragma once

#include "descriptors.hpp"
#include "swap_chain.hpp"
namespace Shard3D {

	class SharedPools{
	public: 
// (0 = global, 1 = per-material static, 2 = per-material dynamic, 3 = per-draw), 
// https://www.reddit.com/r/gamedev/comments/kybh22/vulkan_material_system_descriptor_management_and/

		_S3D_GVAR std::unique_ptr<EngineDescriptorPool> globalPool{};
		_S3D_GVAR std::unique_ptr<EngineDescriptorPool> staticMaterialPool{};
		_S3D_GVAR std::unique_ptr<EngineDescriptorPool> dynamicMaterialPool{};
		_S3D_GVAR std::vector<std::unique_ptr<EngineDescriptorPool>> drawPools;

		_S3D_GVAR void constructPools(EngineDevice& device) {
			// Global pool
			globalPool = EngineDescriptorPool::Builder(device)
				.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
				.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
				.build();

			// Draw pools
			drawPools.resize(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
			auto framePoolBuilder = EngineDescriptorPool::Builder(device)
				.setMaxSets(1024)
				.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024)
				.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024)
				.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024)
				.setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
			for (int i = 0; i < drawPools.size(); i++) {
				drawPools[i] = framePoolBuilder.build();
			}
		}

		_S3D_GVAR void destructPools() {
			globalPool = nullptr;
			staticMaterialPool = nullptr;
			dynamicMaterialPool = nullptr;
			for (int i = 0; i < drawPools.size(); i++) {
				drawPools[i] = nullptr;
			}		
		}
	};
}