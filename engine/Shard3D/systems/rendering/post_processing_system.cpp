#include "../../s3dpch.h" 

#include "post_processing_system.h"
#include "../computational/shader_system.h"
#include "../../core/rendering/render_pass.h"
#include "../../vulkan_abstr.h"
#include "../../ecs.h"
#include "../../core/misc/graphics_settings.h"
#include "../buffers/material_system.h"

namespace Shard3D {
	static bool setVkDescriptorImageInfo(VkDescriptorImageInfo* descriptor, FrameBufferAttachment* attachment) {
		if (attachment == nullptr) return false;
		descriptor->imageLayout = attachment->getDescription().finalLayout;
		descriptor->imageView = attachment->getImageView();
		descriptor->sampler = attachment->getSampler();
		return true;
	}

	PostProcessingSystem::PostProcessingSystem(EngineDevice& device, VkRenderPass presentingRenderPass, PostProcessingGBufferInput imageInput) : engineDevice(device) {
		setVkDescriptorImageInfo(&ppoDescriptor_BaseRenderedScene, imageInput.baseRenderedScene);
		bool x1 = setVkDescriptorImageInfo(&ppoDescriptor_PositionSceneInfo, imageInput.positionSceneInfo);
		bool x2 = setVkDescriptorImageInfo(&ppoDescriptor_NormalSceneInfo, imageInput.normalSceneInfo);
		bool x3 = setVkDescriptorImageInfo(&ppoDescriptor_MaterialSceneInfo, imageInput.materialSceneInfo);

		EngineDescriptorSetLayout::Builder builder = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		if (x1)
			builder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
		if (x2)
			builder.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
		if (x3)
			builder.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);

		ppo_Layout = builder.build();

		EngineDescriptorWriter writer = EngineDescriptorWriter(*ppo_Layout, *SharedPools::staticMaterialPool)
			.writeImage(0, &ppoDescriptor_BaseRenderedScene); 
		if (x1)
			writer.writeImage(1, &ppoDescriptor_PositionSceneInfo);
		if (x2)
			writer.writeImage(2, &ppoDescriptor_NormalSceneInfo);
		if (x3)
			writer.writeImage(3, &ppoDescriptor_MaterialSceneInfo);

		writer.build(ppo_InputDescriptorSet);
		MaterialSystem::setRenderedSceneImageLayout(ppo_Layout->getDescriptorSetLayout());

		createPipelineLayout();
		createPipelines(presentingRenderPass);
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

	void PostProcessingSystem::createPipelines(VkRenderPass renderPass) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		gammaCorrectionShaderPipeline = make_uPtr<ComputePipeline>(
				engineDevice,
				pipelineLayout,
				"assets/shaderdata/post_processing/gamma_correction.comp.spv");

		GraphicsPipelineConfigInfo pipelineConfig{};
		GraphicsPipeline::pipelineConfig(pipelineConfig)
			.defaultGraphicsPipelineConfigInfo()
			.forceSampleCount(VK_SAMPLE_COUNT_1_BIT)
			.setCullMode(VK_CULL_MODE_FRONT_BIT)
			.disableDepthTest();

		pipelineConfig.pipelineLayout = pipelineLayout;
		pipelineConfig.renderPass = renderPass;
		debanderShaderPipeline = make_uPtr<GraphicsPipeline>(
			engineDevice,
			"assets/shaderdata/fullscreen_quad.vert.spv",
			GraphicsSettings::getRuntimeInfo().is10BitColor ? "assets/shaderdata/post_processing/debander_10bit.frag.spv" : "assets/shaderdata/post_processing/debander.frag.spv",
			pipelineConfig
		);
	}

	void PostProcessingSystem::render(FrameInfo& frameInfo) {
		for (auto& material : frameInfo.activeLevel->getPossessedCameraActor().getComponent<Components::CameraComponent>().postProcessMaterials) {
			material.dispatch(frameInfo.commandBuffer, ppo_InputDescriptorSet);
		}
	}

	void PostProcessingSystem::renderGammaCorrection(FrameInfo& frameInfo) {
		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_COMPUTE,
			pipelineLayout,
			0,
			1,
			&ppo_InputDescriptorSet,
			0,
			nullptr
		);
		gammaCorrectionShaderPipeline->bindCompute(frameInfo.commandBuffer);
		vkCmdDispatch(frameInfo.commandBuffer, 1920 / 16, 1080 / 16 + 1, 1);
	}

	void PostProcessingSystem::renderImageFlipForPresenting(FrameInfo& frameInfo) {
		debanderShaderPipeline->bind(frameInfo.commandBuffer);
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
	void PPO_Material::dispatch(VkCommandBuffer commandBuffer) {
		pipeline->bindCompute(commandBuffer);
		vkCmdDispatch(commandBuffer, workgroups.x, workgroups.y, workgroups.z);
	}
}