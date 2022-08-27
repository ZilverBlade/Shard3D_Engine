#include "../../s3dpch.h" 

#include "post_processing_system.h"
#include "../../core/vulkan_api/pipeline.h"
#include "../../core/vulkan_api/descriptors.h"

namespace Shard3D {
	PostProcessingSystem::PostProcessingSystem(EngineDevice& device, VkRenderPass renderPass, VkImageView inputImageView, VkSampler inputSampler) : engineDevice(device) {
		ppo_InputImageInfo.sampler = inputSampler;
		ppo_InputImageInfo.imageView = inputImageView;
		ppo_InputImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		ppo_Layout =
			EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		ppo_Layout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		EngineDescriptorWriter(*ppo_Layout, *SharedPools::staticMaterialPool)
			.writeImage(0, &ppo_InputImageInfo)
			.build(ppo_InputDescriptorSet);


		createPipelineLayout();
		createPipeline(renderPass);
	}

	PostProcessingSystem::~PostProcessingSystem() {
		
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

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::pipelineConfig(pipelineConfig)
			.defaultPipelineConfigInfo()
			.forceSampleCount(VK_SAMPLE_COUNT_1_BIT)
			.setCullingMode(VK_CULL_MODE_FRONT_BIT);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = make_uPtr<EnginePipeline>(
			engineDevice,
			"assets/shaderdata/fullscreen_quad.vert.spv",
			"assets/shaderdata/post_processing/color_correction.frag.spv",
			pipelineConfig
		);
	}

	void PostProcessingSystem::render(FrameInfo& frameInfo) {
		enginePipeline->bind(frameInfo.commandBuffer);
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