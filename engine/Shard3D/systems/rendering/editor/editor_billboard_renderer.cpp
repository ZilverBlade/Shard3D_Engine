#include "../../../s3dpch.h" 
#include <glm/gtc/constants.hpp>

#include "editor_billboard_renderer.h"
#include "../../../core/asset/assetmgr.h"
#include "../../../core/ecs/components.h"

namespace Shard3D {

	struct Billboard {
		glm::vec4 position{};
	};

	_EditorBillboardRenderer::_EditorBillboardRenderer(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	_EditorBillboardRenderer::~_EditorBillboardRenderer() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void _EditorBillboardRenderer::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(Billboard);
		
		billboardSystemLayout =
			EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			globalSetLayout,
			billboardSystemLayout->getDescriptorSetLayout()
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}

	void _EditorBillboardRenderer::createPipeline(VkRenderPass renderPass) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		GraphicsPipelineConfigInfo pipelineConfig{};
		GraphicsPipeline::pipelineConfig(pipelineConfig)
			.defaultGraphicsPipelineConfigInfo()
			.setCullMode(VK_CULL_MODE_BACK_BIT)
			.setSubpass(1);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		graphicsPipeline = make_uPtr<GraphicsPipeline>(
			engineDevice,
			"assets/shaderdata/_editor/editor_svabill.vert.spv",
			"assets/shaderdata/_editor/editor_svabill.frag.spv",
			pipelineConfig
		);
	}

	void _EditorBillboardRenderer::render(FrameInfo &frameInfo) {
		graphicsPipeline->bind(frameInfo.commandBuffer);
		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr
		);
		renderComponent<Components::AudioComponent>("component.audio", frameInfo);
		renderComponent<Components::PointlightComponent>("component.light.point", frameInfo);
		renderComponent<Components::SpotlightComponent>("component.light.spot", frameInfo);
		renderComponent<Components::DirectionalLightComponent>("component.light.directional", frameInfo);

		// TODO: render camera component as mesh (special case)
	}

	template<typename T>
	inline void _EditorBillboardRenderer::renderComponent(const char* iconName, FrameInfo& frameInfo) {
		auto imageInfo = _special_assets::_editor_icons.at(iconName)->getImageInfo();
		VkDescriptorSet descriptorSet1;
		EngineDescriptorWriter(*billboardSystemLayout, frameInfo.perDrawDescriptorPool)
			.writeImage(1, &imageInfo)
			.build(descriptorSet1);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			1,  // first set
			1,  // set count
			&descriptorSet1,
			0,
			nullptr);

		frameInfo.activeLevel->registry.view<T, Components::TransformComponent>().each([&](auto light, auto transform) {
			Billboard push{};
		
			push.position = transform.transformMatrix[3];

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(Billboard),
				&push
			);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		});
	}

}