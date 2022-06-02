#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "directional_light_system.hpp"
#include <stdexcept>
#include <array>
#include <map>

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
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void DirectionalLightSystem::createPipeline(VkRenderPass renderPass) {
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
			"shaders/directional_light.vert.spv",
			"shaders/directional_light.frag.spv",
			pipelineConfig
		);
	}

	void DirectionalLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Scene>& scene) {
		int lightIndex = 0;
		scene->eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, scene.get() };
			if (!actor) return;
				
			if (actor.hasComponent<Components::DirectionalLightComponent>()) {
				ubo.directionalLights[lightIndex].position = glm::vec4(actor.getComponent<Components::TransformComponent>().translation, 1.f);
				ubo.directionalLights[lightIndex].color = glm::vec4(actor.getComponent<Components::DirectionalLightComponent>().color, actor.getComponent<Components::DirectionalLightComponent>().lightIntensity);
				ubo.directionalLights[lightIndex].direction = glm::vec4(actor.getComponent<Components::TransformComponent>().rotation, 1.f);
				ubo.directionalLights[lightIndex].specularMod = actor.getComponent<Components::DirectionalLightComponent>().specularMod;
				lightIndex += 1;
			}
		});
		ubo.numDirectionalLights = lightIndex;
	}

	void DirectionalLightSystem::render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Scene>& scene) {
		enginePipeline->bind(frameInfo.commandBuffer);

		scene->eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, scene.get() };
		if (!actor) return;
		// copy light to ubo
		if (actor.hasComponent<Components::DirectionalLightComponent>()) {

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

			DirectionalLightPushConstants push{};
			push.position = glm::vec4(actor.getComponent<Components::TransformComponent>().translation, 1.f);
			push.color = glm::vec4(actor.getComponent<Components::DirectionalLightComponent>().color, actor.getComponent<Components::DirectionalLightComponent>().lightIntensity);
			push.direction = glm::vec4(actor.getComponent<Components::TransformComponent>().rotation, 1.f);
			push.specularMod = actor.getComponent<Components::DirectionalLightComponent>().specularMod;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(DirectionalLightPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
		});
		
	}
}