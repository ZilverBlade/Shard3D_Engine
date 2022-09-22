#include "../../s3dpch.h" 
#include <glm/gtc/constants.hpp>

#include "billboard_system.h"
#include "../../core/ecs/components.h"
#include "../../core/asset/assetmgr.h"
namespace Shard3D::Systems {

	struct BillboardPushConstants {
		glm::vec4 translation;
		glm::vec4 rotation; // maybe roll?
		glm::vec4 scale;
	};

	BillboardRenderSystem::BillboardRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	BillboardRenderSystem::~BillboardRenderSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}

	void BillboardRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(BillboardPushConstants);

		billboardSystemLayout =
			EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
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

	void BillboardRenderSystem::createPipeline(VkRenderPass renderPass) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		GraphicsPipelineConfigInfo pipelineConfig{};
		GraphicsPipeline::pipelineConfig(pipelineConfig)
			.defaultGraphicsPipelineConfigInfo()
			//.enableAlphaBlending(VK_BLEND_OP_ADD)
			.setCullMode(VK_CULL_MODE_BACK_BIT)
			.setSubpass(1);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout; // support only view plane aligned atm
		graphicsPipeline = make_uPtr<GraphicsPipeline>(
			engineDevice,
			"assets/shaderdata/billboard_sva.vert.spv",
			"assets/shaderdata/billboard_sva.frag.spv",
			pipelineConfig
		);
	}

	void BillboardRenderSystem::render(FrameInfo& frameInfo) {
		graphicsPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		frameInfo.activeLevel->registry.view<Components::BillboardComponent, Components::TransformComponent>().each([&](auto billboard, auto transform) {
			auto imageInfo = ResourceHandler::retrieveTexture(billboard.asset)->getImageInfo();
			VkDescriptorSet descriptorSet1;
			EngineDescriptorWriter(*billboardSystemLayout, frameInfo.perDrawDescriptorPool)
				.writeImage(0, &imageInfo)
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
			glm::vec3 s = transform.getScale();
			glm::vec3 t = transform.getTranslation();

			BillboardPushConstants push{};
			push.translation = glm::vec4(t.x, t.z, t.y, 1.f);
			push.scale = glm::vec4(s.x, s.z, 0.f, 1.f);

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(BillboardPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		});

	}
}