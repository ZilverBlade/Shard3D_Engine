#pragma once
#pragma deprecated

#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/ecs/level.h"

namespace Shard3D {
	inline namespace Systems {
		class ForwardRenderSystem {
		public:
#if 0
			ForwardRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~ForwardRenderSystem();

			ForwardRenderSystem(const ForwardRenderSystem&) = delete;
			ForwardRenderSystem& operator=(const ForwardRenderSystem&) = delete;

			void renderForward(FrameInfo& frameInfo);
			void renderForwardOld(FrameInfo& frameInfo);
			void renderForwardNew(FrameInfo& frameInfo);
		private:
			void renderClass(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags);
			void _0_renderClass(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags);

			EngineDevice& engineDevice;
#endif
		};
	}
}