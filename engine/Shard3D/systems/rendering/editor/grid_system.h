#pragma once

#include "../../../s3dstd.h"
#include "../../../vulkan_abstr.h"

namespace Shard3D {
	class GridSystem {
	public:
		GridSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~GridSystem();

		GridSystem(const GridSystem&) = delete;
		GridSystem& operator=(const GridSystem&) = delete;

		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice* engineDevice;

		uPtr<GraphicsPipeline> graphicsPipeline;
		VkPipelineLayout pipelineLayout;
	};
}