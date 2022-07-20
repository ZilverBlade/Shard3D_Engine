#include "../s3dtpch.h" 
#include <glm/gtc/constants.hpp>

#include "editor_billboard_renderer.hpp"
#include "../wb3d/assetmgr.hpp"

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
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			"assets/shaderdata/_editor/editor_svabill.vert.spv",
			"assets/shaderdata/_editor/editor_svabill.frag.spv",
			pipelineConfig
		);
	}

	void _EditorBillboardRenderer::render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Level>& level) {
		enginePipeline->bind(frameInfo.commandBuffer);
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
		renderComponent<Components::AudioComponent>("component.audio", frameInfo, level);
		renderComponent<Components::PointlightComponent>("component.light.point", frameInfo, level);
		renderComponent<Components::SpotlightComponent>("component.light.spot", frameInfo, level);
		renderComponent<Components::DirectionalLightComponent>("component.light.directional", frameInfo, level);

		// TODO: render camera component as mesh (special case)
	}

	template<typename T>
	inline void _EditorBillboardRenderer::renderComponent(const char* iconName, FrameInfo& frameInfo, std::shared_ptr<wb3d::Level>& level) {
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

		level->registry.view<T, Components::TransformComponent>().each([&](auto light, auto transform) {
			Billboard push{};
			push.position = glm::vec4(transform.translation, 1.f);

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