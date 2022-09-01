#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  

namespace Shard3D {
	class FrameBufferAttachment;
	struct PostProcessingGBufferInput {
		FrameBufferAttachment* baseRenderedScene;
		FrameBufferAttachment* depthBufferSceneInfo;
		FrameBufferAttachment* positionSceneInfo;
		FrameBufferAttachment* normalSceneInfo;
	};
	class PostProcessingSystem {
	public:
		PostProcessingSystem(EngineDevice& device, VkRenderPass renderPass, PostProcessingGBufferInput imageInput);
		~PostProcessingSystem();

		PostProcessingSystem(const PostProcessingSystem&) = delete;
		PostProcessingSystem& operator=(const PostProcessingSystem&) = delete;

		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		
		EngineDevice& engineDevice;

		uPtr<GraphicsPipeline> graphicsPipeline;
		VkDescriptorSet ppo_InputDescriptorSet{};

		VkDescriptorImageInfo ppoDescriptor_BaseRenderedScene;
		VkDescriptorImageInfo ppoDescriptor_DepthBufferSceneInfo;
		VkDescriptorImageInfo ppoDescriptor_PositionSceneInfo;
		VkDescriptorImageInfo ppoDescriptor_NormalSceneInfo;

		uPtr<EngineDescriptorSetLayout> ppo_Layout{};
		VkPipelineLayout pipelineLayout{};
	};

}