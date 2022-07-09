#include "../s3dtpch.h" 
#include <glm/gtc/constants.hpp>

#include "directional_light_system.hpp"

namespace Shard3D {

	struct DirectionalLightPushConstants {
		glm::vec4 position{};
		glm::vec4 color{};
		glm::vec4 direction{}; 
		alignas(16) float specularMod{};
	};

	DirectionalLightSystem::DirectionalLightSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	DirectionalLightSystem::~DirectionalLightSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void DirectionalLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(DirectionalLightPushConstants);
		
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

	void DirectionalLightSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
		EnginePipeline::enableAlphaBlending(pipelineConfig, VK_BLEND_OP_ADD);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			"assets/shaders/directional_light.vert.spv",
			"assets/shaders/directional_light.frag.spv",
			pipelineConfig
		);
	}

	void DirectionalLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
		int lightIndex = 0;
		level->registry.view<Components::DirectionalLightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {
			ubo.directionalLights[lightIndex].position = glm::vec4(transform.translation, 1.f);
			ubo.directionalLights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
			ubo.directionalLights[lightIndex].direction = glm::vec4(transform.rotation, 1.f);
			ubo.directionalLights[lightIndex].specularMod = light.specularMod;
			lightIndex += 1;
		});
		ubo.numDirectionalLights = lightIndex;
	}

	void DirectionalLightSystem::render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Level>& level) {
		enginePipeline->bind(frameInfo.commandBuffer);

		level->registry.view<Components::DirectionalLightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {
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
			DirectionalLightPushConstants push{};
			push.position = glm::vec4(transform.translation, 1.f);
			push.color = glm::vec4(light.color, light.lightIntensity);
			push.direction = glm::vec4(transform.rotation, 1.f);
			push.specularMod = light.specularMod;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(DirectionalLightPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		});
		
	}
}