#include "../s3dtpch.h" 
#include <glm/gtc/constants.hpp>

#include "pointlight_system.hpp"

namespace Shard3D {

	struct PointlightPushConstants {
		glm::vec4 position{};
		glm::vec4 color{};
		glm::vec4 attenuationMod{};
		alignas(16) float radius;
		alignas(16) float specularMod{};
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
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointlightPushConstants);
		
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

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
		EnginePipeline::enableAlphaBlending(pipelineConfig, VK_BLEND_OP_ADD);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			"assets/shaders/pointlight.vert.spv",
			"assets/shaders/pointlight.frag.spv",
			pipelineConfig
		);
	}

	void PointlightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
#ifndef _DEPLOY
		if (ubo.numPointlights > MAX_POINTLIGHTS) {
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

		level->registry.view<Components::PointlightComponent, Components::TransformComponent>().each([=](auto light, auto transform) {
			// copy light to ubo
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
				CSimpleIniA ini;
				ini.SetUnicode();
				ini.LoadFile(ENGINE_SETTINGS_PATH);

				if (light.attenuationMod != glm::vec3(0.f, 0.f, 1.f) && ini.GetBoolValue("WARNINGS", "warn.NotInverseSquareAttenuation")) {
					SHARD3D_WARN("NotInverseSquareAttenuation: \"Pointlight in level does not obey inverse square law\"");
				}

				PointlightPushConstants push{};
				push.position = glm::vec4(transform.translation, 1.f);
				push.color = glm::vec4(light.color, light.lightIntensity);
				push.radius = light.radius / 10;
				push.attenuationMod = glm::vec4(light.attenuationMod, 0.f);
				push.specularMod = light.specularMod;

				vkCmdPushConstants(
					frameInfo.commandBuffer,
					pipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(PointlightPushConstants),
					&push
				);
				vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		});
	}
}