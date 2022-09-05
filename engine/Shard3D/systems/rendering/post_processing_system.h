#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  

namespace Shard3D {
	class RenderPass;
	class FrameBuffer;
	class FrameBufferAttachment;
	struct PostProcessingGBufferInput {
		FrameBufferAttachment* baseRenderedScene;
		FrameBufferAttachment* depthBufferSceneInfo;
		FrameBufferAttachment* positionSceneInfo;
		FrameBufferAttachment* normalSceneInfo;
	};
	class PostProcessingSystem {
	public:
		PostProcessingSystem(EngineDevice& device, VkRenderPass swapchainRenderPass, PostProcessingGBufferInput imageInput);
		~PostProcessingSystem();

		PostProcessingSystem(const PostProcessingSystem&) = delete;
		PostProcessingSystem& operator=(const PostProcessingSystem&) = delete;

		void render(FrameInfo& frameInfo);
		void renderGammaCorrectionForSwapchainRenderPass(FrameInfo& frameInfo);

		FrameBufferAttachment* getFrameBufferAttachment() { return ppoColorFramebufferAttachment; }
	private:
		void createOffscreenRenderPass();
		void createPipelineLayout();
		void createPipelines(VkRenderPass renderPass);
		
		FrameBufferAttachment* ppoColorFramebufferAttachment;
		FrameBuffer* ppoFrameBuffer;
		RenderPass* ppoRenderpass;

		EngineDevice& engineDevice;

		uPtr<GraphicsPipeline> gammaCorrectionShaderPipeline;
		uPtr<GraphicsPipeline> hdrShaderPipeline;
		uPtr<GraphicsPipeline> bloomShaderPipeline;
		uPtr<GraphicsPipeline> debanderShaderPipeline;
		uPtr<GraphicsPipeline> entireShaderPipeline;

		VkDescriptorSet ppo_InputDescriptorSet{};

		VkDescriptorImageInfo ppoDescriptor_BaseRenderedScene;
		VkDescriptorImageInfo ppoDescriptor_DepthBufferSceneInfo;
		VkDescriptorImageInfo ppoDescriptor_PositionSceneInfo;
		VkDescriptorImageInfo ppoDescriptor_NormalSceneInfo;

		uPtr<EngineDescriptorSetLayout> ppo_Layout{};
		uPtr<EngineBuffer> ssboImageBuffer;

		VkPipelineLayout pipelineLayout{};
	};

}