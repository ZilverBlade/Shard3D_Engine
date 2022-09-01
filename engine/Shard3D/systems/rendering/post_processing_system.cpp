#include "../../s3dpch.h" 

#include "post_processing_system.h"
#include "../../core/vulkan_api/pipeline.h"
#include "../../core/vulkan_api/descriptors.h"
#include "../../core/rendering/renderpass.h"

namespace Shard3D {
	static bool setVkDescriptorImageInfo(VkDescriptorImageInfo* descriptor, FrameBufferAttachment* attachment) {
		if (attachment == nullptr) return false;
		descriptor->imageLayout = attachment->getDescription().finalLayout;
		descriptor->imageView = attachment->getImageView();
		descriptor->sampler = attachment->getSampler();
		return true;
	}

	PostProcessingSystem::PostProcessingSystem(EngineDevice& device, VkRenderPass renderPass, PostProcessingGBufferInput imageInput) : engineDevice(device) {
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
		createPipeline(renderPass);
	}

	PostProcessingSystem::~PostProcessingSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
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

	void PostProcessingSystem::createPipeline(VkRenderPass renderPass) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		GraphicsPipelineConfigInfo pipelineConfig{};
		GraphicsPipeline::pipelineConfig(pipelineConfig)
			.defaultGraphicsPipelineConfigInfo()
			.forceSampleCount(VK_SAMPLE_COUNT_1_BIT)
			.setCullMode(VK_CULL_MODE_FRONT_BIT)
			.disableDepthTest();

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		graphicsPipeline = make_uPtr<GraphicsPipeline>(
			engineDevice,
			"assets/shaderdata/fullscreen_quad.vert.spv",
			"assets/shaderdata/post_processing/bloom.frag.spv",
			pipelineConfig
		);
	}

	void PostProcessingSystem::render(FrameInfo& frameInfo) {
		graphicsPipeline->bind(frameInfo.commandBuffer);
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