#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/vulkan_api/pipeline_compute.h"

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

	struct PPO_Material {
		PPO_Material(sPtr<ComputePipeline>&& _pipeline, glm::ivec3 _workgroups) : pipeline(_pipeline), workgroups(_workgroups) {}
		void dispatch(VkCommandBuffer commandBuffer);
		sPtr<ComputePipeline> pipeline;
		glm::ivec3 workgroups;
	};

	class PostProcessingSystem {
	public:
		PostProcessingSystem(EngineDevice& device, VkRenderPass swapchainpresentingRenderPassRenderPass, PostProcessingGBufferInput imageInput, bool doGammaCorrection);
		~PostProcessingSystem();

		PostProcessingSystem(const PostProcessingSystem&) = delete;
		PostProcessingSystem& operator=(const PostProcessingSystem&) = delete;

		void render(FrameInfo& frameInfo);
		void renderImageFlipForPresenting(FrameInfo& frameInfo);
	private:
		void createPipelineLayout();
		void createPipelines(VkRenderPass renderPass);
		
		EngineDevice& engineDevice;

		uPtr<GraphicsPipeline> debanderShaderPipeline;
		uPtr<ComputePipeline> hdrShaderPipeline;
		uPtr<ComputePipeline> bloomShaderPipeline;

		VkDescriptorSet ppo_InputDescriptorSet{};

		VkDescriptorImageInfo ppoDescriptor_BaseRenderedScene;
		VkDescriptorImageInfo ppoDescriptor_DepthBufferSceneInfo;
		VkDescriptorImageInfo ppoDescriptor_PositionSceneInfo;
		VkDescriptorImageInfo ppoDescriptor_NormalSceneInfo;

		uPtr<EngineDescriptorSetLayout> ppo_Layout{};

		VkPipelineLayout pipelineLayout{};

		std::vector<PPO_Material> postProcessMaterials{};

		bool doGammaCorrectionEX;
	};

}