#pragma once

#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/ecs/level.h"

namespace Shard3D {
	inline namespace Rendering {
		class FrameBufferAttachment;
		class FrameBuffer;
		class RenderPassAdvanced;
	}
	inline namespace Systems {
		struct GBufferInputData {
			FrameBufferAttachment* baseRenderedScene;
			FrameBufferAttachment* depthSceneInfo;
			FrameBufferAttachment* normalSceneInfo;
			FrameBufferAttachment* materialDiffuseSceneInfo;
			FrameBufferAttachment* materialParamSceneInfo;
		};
		class DeferredRenderSystem {
		public:
			DeferredRenderSystem(EngineDevice& device, VkDescriptorSetLayout globalSetLayout);
			~DeferredRenderSystem();

			DELETE_COPY(DeferredRenderSystem)

			void renderDeferred(FrameInfo& frameInfo);
			void renderForward(FrameInfo& frameInfo);
			void renderDeferredNew(FrameInfo& frameInfo);
			void renderForwardNew(FrameInfo& frameInfo);

			void beginRenderPass(FrameInfo& frameInfo);
			void endRenderPass(FrameInfo& frameInfo);

			inline GBufferInputData&& getGBufferAttachments() {
				return { 			
					finalFramebufferAttachment,
					depthFramebufferAttachment,
					normalFramebufferAttachment,
					materialDiffuseFramebufferAttachment,
					materialParamFramebufferAttachment		
				};
			}

			void resize(glm::ivec3 newSize);
			inline RenderPassAdvanced* getRenderPass() { return deferredRenderpass; }
			inline uPtr<EngineDescriptorSetLayout>& getGBufferAttachmentSubpassInputLayout() { return subpassInputLayout; }
			inline VkDescriptorSet& getGBufferAttachmentSubpassInputDescriptorSet() { return subpassInputDescriptorSet; }
		private:
			void createRenderPass();
			void createFramebuffer();

			void renderClassDeferred(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags);
			void renderClassForward(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags);

			FrameBufferAttachment 
				*finalFramebufferAttachment, 
				*depthFramebufferAttachment,
				*normalFramebufferAttachment,
				*materialDiffuseFramebufferAttachment,
				*materialParamFramebufferAttachment;

			FrameBuffer* deferredFrameBuffer;
			RenderPassAdvanced* deferredRenderpass;

			uPtr<EngineDescriptorSetLayout> subpassInputLayout;
			VkDescriptorSet subpassInputDescriptorSet{};
			EngineDevice& engineDevice;
		};
	}
}