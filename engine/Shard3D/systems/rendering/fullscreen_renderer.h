#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  


namespace Shard3D {
	class FullscreenRenderer {
	public:
		FullscreenRenderer(VkRenderPass renderPass);
		~FullscreenRenderer();

		FullscreenRenderer(const FullscreenRenderer&) = delete;
		FullscreenRenderer& operator=(const FullscreenRenderer&) = delete;

		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		uPtr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		uPtr<EngineDescriptorSetLayout> quadSystemLayout;
		VkDescriptorSet screenImageDescriptorSet;
		VkDescriptorImageInfo imageInfo{};
	};

}