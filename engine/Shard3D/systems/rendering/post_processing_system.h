#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  

namespace Shard3D {
	class PostProcessingSystem {
	public:
		PostProcessingSystem(EngineDevice& device, VkRenderPass renderPass, VkImageView inputImageView, VkSampler inputSampler);
		~PostProcessingSystem();

		PostProcessingSystem(const PostProcessingSystem&) = delete;
		PostProcessingSystem& operator=(const PostProcessingSystem&) = delete;

		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		
		EngineDevice& engineDevice;

		uPtr<EnginePipeline> enginePipeline;
		VkDescriptorSet ppo_InputDescriptorSet{};
		VkDescriptorImageInfo ppo_InputImageInfo{};

		uPtr<EngineDescriptorSetLayout> ppo_Layout{};
		VkPipelineLayout pipelineLayout{};
	};

}