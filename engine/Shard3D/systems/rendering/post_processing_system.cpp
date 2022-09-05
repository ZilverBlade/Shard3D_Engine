#include "../../s3dpch.h" 

#include "post_processing_system.h"
#include "../computational/shader_system.h"
#include "../../core/vulkan_api/pipeline.h"
#include "../../core/vulkan_api/descriptors.h"
#include "../../core/rendering/render_pass.h"
#include "../../core/rendering/frame_buffer.h"

namespace Shard3D {
	static bool setVkDescriptorImageInfo(VkDescriptorImageInfo* descriptor, FrameBufferAttachment* attachment) {
		if (attachment == nullptr) return false;
		descriptor->imageLayout = attachment->getDescription().finalLayout;
		descriptor->imageView = attachment->getImageView();
		descriptor->sampler = attachment->getSampler();
		return true;
	}

	PostProcessingSystem::PostProcessingSystem(EngineDevice& device, VkRenderPass swapchainRenderPass, PostProcessingGBufferInput imageInput) : engineDevice(device) {
		createOffscreenRenderPass();

		setVkDescriptorImageInfo(&ppoDescriptor_BaseRenderedScene, imageInput.baseRenderedScene);
		bool x1 = setVkDescriptorImageInfo(&ppoDescriptor_DepthBufferSceneInfo, imageInput.depthBufferSceneInfo);
		bool x2 = setVkDescriptorImageInfo(&ppoDescriptor_PositionSceneInfo, imageInput.positionSceneInfo);
		bool x3 = setVkDescriptorImageInfo(&ppoDescriptor_NormalSceneInfo, imageInput.normalSceneInfo);

		EngineDescriptorSetLayout::Builder builder = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		if (x1)
			builder.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		if (x2)
			builder.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		if (x3)
			builder.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

		ppo_Layout = builder.build();

		EngineDescriptorWriter writer = EngineDescriptorWriter(*ppo_Layout, *SharedPools::staticMaterialPool)
			.writeImage(0, &ppoDescriptor_BaseRenderedScene);
		if (x1)
			writer.writeImage(1, &ppoDescriptor_DepthBufferSceneInfo);
		if (x2)
			writer.writeImage(2, &ppoDescriptor_PositionSceneInfo);
		if (x3)
			writer.writeImage(3, &ppoDescriptor_NormalSceneInfo);

		writer.build(ppo_InputDescriptorSet);


		createPipelineLayout();
		createPipelines(swapchainRenderPass);
	}

	PostProcessingSystem::~PostProcessingSystem() {
		delete ppoFrameBuffer;
		delete ppoRenderpass;
		delete ppoColorFramebufferAttachment;

		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}

	void PostProcessingSystem::createOffscreenRenderPass() {
		{ // Post processing
			ppoColorFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
			VK_FORMAT_B8G8R8A8_SRGB,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color);

			AttachmentInfo colorAttachmentInfo{};
			colorAttachmentInfo.frameBufferAttachment = ppoColorFramebufferAttachment;
			colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			ppoRenderpass = new RenderPass(
				engineDevice, {
				colorAttachmentInfo
				});

			ppoFrameBuffer = new FrameBuffer(engineDevice, ppoRenderpass->getRenderPass(), { ppoColorFramebufferAttachment });
		}
	}

	void PostProcessingSystem::createPipelineLayout() {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			ppo_Layout->getDescriptorSetLayout()
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}

	void PostProcessingSystem::createPipelines(VkRenderPass renderPass) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		GraphicsPipelineConfigInfo pipelineConfig{};
		GraphicsPipeline::pipelineConfig(pipelineConfig)
			.defaultGraphicsPipelineConfigInfo()
			.forceSampleCount(VK_SAMPLE_COUNT_1_BIT)
			.setCullMode(VK_CULL_MODE_FRONT_BIT)
			.disableDepthTest();

		pipelineConfig.renderPass = ppoRenderpass->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		
		//hdrShaderPipeline = make_uPtr<GraphicsPipeline>(
		//	engineDevice,
		//	"assets/shaderdata/fullscreen_quad.vert.spv",
		//	"assets/shaderdata/post_processing/hdr.frag.spv",
		//	pipelineConfig
		//	);
		//bloomShaderPipeline = make_uPtr<GraphicsPipeline>(
		//	engineDevice,
		//	"assets/shaderdata/fullscreen_quad.vert.spv",
		//	"assets/shaderdata/post_processing/bloom.frag.spv",
		//	pipelineConfig
		//	);
		//debanderShaderPipeline = make_uPtr<GraphicsPipeline>(
		//	engineDevice,
		//	"assets/shaderdata/fullscreen_quad.vert.spv",
		//	"assets/shaderdata/post_processing/debander.frag.spv",
		//	pipelineConfig
		//	);
		//
		entireShaderPipeline = make_uPtr<GraphicsPipeline>(
			engineDevice,
			"assets/shaderdata/fullscreen_quad.vert.spv",
			"assets/shaderdata/post_processing/combined.frag.spv",
			pipelineConfig
			);

		//pipelineConfig.renderPass = renderPass;
		//
		//gammaCorrectionShaderPipeline = make_uPtr<GraphicsPipeline>(
		//	engineDevice,
		//	"assets/shaderdata/fullscreen_quad.vert.spv",
		//	"assets/shaderdata/post_processing/gamma_correction.frag.spv",
		//	pipelineConfig
		//	);
	}

	void PostProcessingSystem::render(FrameInfo& frameInfo) {
		ppoRenderpass->beginRenderPass(frameInfo, ppoFrameBuffer);
		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&ppo_InputDescriptorSet,
			0,
			nullptr
		);

		entireShaderPipeline->bind(frameInfo.commandBuffer);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);

		ppoRenderpass->endRenderPass(frameInfo);
	}

	void PostProcessingSystem::renderGammaCorrectionForSwapchainRenderPass(FrameInfo& frameInfo) {
		gammaCorrectionShaderPipeline->bind(frameInfo.commandBuffer);
		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&ppo_InputDescriptorSet,
			0,
			nullptr
		);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}