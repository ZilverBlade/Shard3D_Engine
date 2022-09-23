#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/vulkan_api/pipeline_compute.h"

namespace Shard3D {
	inline namespace Rendering {
		class RenderPass;
		class FrameBuffer;
		class FrameBufferAttachment;
	}
	inline namespace Systems {
		struct GBufferInputData;
		class PostProcessingSystem {
		public:
			PostProcessingSystem(EngineDevice& device, VkRenderPass swapchainpresentingRenderPassRenderPass, GBufferInputData* imageInput);
			~PostProcessingSystem();

			PostProcessingSystem(const PostProcessingSystem&) = delete;
			PostProcessingSystem& operator=(const PostProcessingSystem&) = delete;

			void render(FrameInfo& frameInfo);
			void renderGammaCorrection(FrameInfo& frameInfo);
			void renderImageFlipForPresenting(FrameInfo& frameInfo);
			void updateDescriptors(GBufferInputData* imageInput);
		private:
			void createPipelineLayout();
			void createPipelines(VkRenderPass renderPass);

			EngineDevice& engineDevice;

			uPtr<GraphicsPipeline> debanderShaderPipeline;
			uPtr<ComputePipeline> gammaCorrectionShaderPipeline;

			VkDescriptorSet ppo_InputDescriptorSet{};

			VkDescriptorImageInfo ppoDescriptor_BaseRenderedScene;
			VkDescriptorImageInfo ppoDescriptor_PositionSceneInfo;
			VkDescriptorImageInfo ppoDescriptor_NormalSceneInfo;
			VkDescriptorImageInfo ppoDescriptor_MaterialDiffuseSceneInfo;
			VkDescriptorImageInfo ppoDescriptor_MaterialParamSceneInfo;

			uPtr<EngineDescriptorSetLayout> ppo_Layout{};

			Synchronization syncComputePPO{ SynchronizationType::GraphicsToCompute, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			Synchronization syncComputeDepth{ SynchronizationType::GraphicsToCompute, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT };
			
			VkPipelineLayout pipelineLayout{};
		};
	}
}