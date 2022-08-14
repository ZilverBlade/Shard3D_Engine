#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  

namespace Shard3D {
	class ComputeSystem {
	public:
		ComputeSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ComputeSystem();

		ComputeSystem(const ComputeSystem&) = delete;
		ComputeSystem& operator=(const ComputeSystem&) = delete;

		void render(FrameInfo& frameInfo);
	private:

		void createPipeline(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		VkDescriptorSet descriptorSet1;

		EngineDevice& engineDevice;
		VkDescriptorImageInfo srcImageInfo{};
		VkDescriptorImageInfo dstImageInfo{};
		uPtr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		uPtr<EngineDescriptorSetLayout> computeSystemLayout;
	};

}