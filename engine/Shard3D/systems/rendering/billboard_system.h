#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/ecs/level.h"

namespace Shard3D {
	inline namespace Systems {
		class BillboardRenderSystem {
		public:
			BillboardRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~BillboardRenderSystem();

			BillboardRenderSystem(const BillboardRenderSystem&) = delete;
			BillboardRenderSystem& operator=(const BillboardRenderSystem&) = delete;

			void render(FrameInfo& frameInfo);

		private:

			void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
			void createPipeline(VkRenderPass renderPass);

			EngineDevice& engineDevice;

			uPtr<GraphicsPipeline> graphicsPipeline;
			VkPipelineLayout pipelineLayout;
			uPtr<EngineDescriptorSetLayout> billboardSystemLayout;
		};
	}
}