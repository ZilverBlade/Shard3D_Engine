#pragma once

#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/ecs/level.h"

namespace Shard3D {
	class ForwardRenderSystem {
	public:
		ForwardRenderSystem(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ForwardRenderSystem();

		ForwardRenderSystem(const ForwardRenderSystem&) = delete;
		ForwardRenderSystem& operator=(const ForwardRenderSystem&) = delete;

		void renderForward(FrameInfo &frameInfo);
	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;

		uPtr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		uPtr<EngineDescriptorSetLayout> skyboxLayout;
	};

}