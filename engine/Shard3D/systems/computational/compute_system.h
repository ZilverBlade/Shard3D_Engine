#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/vulkan_api/pipeline_compute.h"
namespace Shard3D {
	class ComputeSystem {
	public:
		ComputeSystem(EngineDevice& device);
		~ComputeSystem();

		ComputeSystem(const ComputeSystem&) = delete;
		ComputeSystem& operator=(const ComputeSystem&) = delete;

		void execute(FrameInfo& frameInfo);
	private:

		void createPipeline();
		VkDescriptorSet descriptorSet1;

		EngineDevice& engineDevice;
		VkDescriptorImageInfo srcImageInfo{};
		VkDescriptorImageInfo dstImageInfo{};
		uPtr<ComputePipeline> computePipeline;
		VkPipelineLayout pipelineLayout;
		uPtr<EngineDescriptorSetLayout> computeSystemLayout;
	};

}