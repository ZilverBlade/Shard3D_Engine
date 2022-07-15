#include "../s3dtpch.h" 
#include <glm/gtc/constants.hpp>

#include "pointlight_system.hpp"
#include "../wb3d/assetmgr.hpp"

namespace Shard3D {

	struct Billboard {
		glm::vec4 position{};
	};

	PointlightSystem::PointlightSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	PointlightSystem::~PointlightSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void PointlightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
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

	void PointlightSystem::createPipeline(VkRenderPass renderPass) {
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

	void PointlightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
#ifndef _DEPLOY
		if (ubo.numPointlights > ENGINE_MAX_POINTLIGHTS) {
			SHARD3D_FATAL("Too many pointlights in level!!!!");
		}
#endif
		int lightIndex = 0;
		level->registry.view<Components::PointlightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {
			ubo.pointlights[lightIndex].position = glm::vec4(transform.translation, 1.f);
			ubo.pointlights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
			ubo.pointlights[lightIndex].attenuationMod = glm::vec4(light.attenuationMod, 0.f);
			ubo.pointlights[lightIndex].specularMod = light.specularMod;
			lightIndex += 1;
		});
		ubo.numPointlights = lightIndex;
	}

	void PointlightSystem::render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Level>& level) {
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

		auto imageInfo = _special_assets::_editor_icons.at("editor.light.point")->getImageInfo();
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


		level->registry.view<Components::PointlightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {			
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