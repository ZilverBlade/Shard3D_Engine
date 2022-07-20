#include "../s3dtpch.h" 

#include "fullscreen_renderer.hpp"

#include "../singleton.hpp"

namespace Shard3D {

	FullscreenRenderer::FullscreenRenderer(VkRenderPass renderPass) {
		createPipelineLayout();
		createPipeline(renderPass);
	}
	FullscreenRenderer::~FullscreenRenderer() {
		vkDestroyPipelineLayout(Singleton::engineDevice.device(), pipelineLayout, nullptr);
	}

	void FullscreenRenderer::createPipelineLayout() {
		quadSystemLayout =
			EngineDescriptorSetLayout::Builder(Singleton::engineDevice)
			.addBinding(10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			quadSystemLayout->getDescriptorSetLayout() 
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(Singleton::engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}

		//imageInfo.sampler = Singleton::mainOffScreen.getSampler();
		//imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = Singleton::mainOffScreen.getImageView();
	}

	void FullscreenRenderer::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout; 
		enginePipeline = std::make_unique<EnginePipeline>(
			Singleton::engineDevice,
			"assets/shaderdata/_output_img.vert.spv",
			"assets/shaderdata/_output_img.frag.spv",
			pipelineConfig
		);
	}

	void FullscreenRenderer::render(FrameInfo& frameInfo) {
		enginePipeline->bind(frameInfo.commandBuffer);

		EngineDescriptorWriter(*quadSystemLayout, frameInfo.perDrawDescriptorPool)
			.writeImage(10, &imageInfo)
			.build(screenImageDescriptorSet);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,  // first set
			1,  // set count
			&screenImageDescriptorSet,
			0,
			nullptr);

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}