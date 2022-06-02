#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "spotlight_system.hpp"
#include <stdexcept>
#include <array>
#include <iostream>
namespace Shard3D {

	struct SpotlightPushConstants {
		glm::vec4 position{};
		glm::vec4 color{};
		glm::vec4 direction{};
		alignas(16) glm::vec2 angle{}; //x = outer, y = inner
		glm::vec4 attenuationMod{};
		alignas(16) float radius;
		alignas(16) float specularMod{};
	};

	SpotlightSystem::SpotlightSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	SpotlightSystem::~SpotlightSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void SpotlightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {		
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SpotlightPushConstants);
		
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

	void SpotlightSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			"shaders/spotlight.vert.spv",
			"shaders/spotlight.frag.spv",
			pipelineConfig
		);
	}

	void SpotlightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Scene>& scene) {
		int lightIndex = 0;
		scene->eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, scene.get() };
		if (!actor) return;

		if (actor.hasComponent<Components::SpotlightComponent>()) {
			ubo.spotlights[lightIndex].position = glm::vec4(actor.getComponent<Components::TransformComponent>().translation, 1.f);
			ubo.spotlights[lightIndex].color = glm::vec4(actor.getComponent<Components::SpotlightComponent>().color, actor.getComponent<Components::SpotlightComponent>().lightIntensity);
			ubo.spotlights[lightIndex].direction = glm::vec4(actor.getComponent<Components::TransformComponent>().rotation, 1.f);
			ubo.spotlights[lightIndex].angle = glm::vec2(actor.getComponent<Components::SpotlightComponent>().outerAngle, actor.getComponent<Components::SpotlightComponent>().innerAngle);
			ubo.spotlights[lightIndex].attenuationMod = actor.getComponent<Components::SpotlightComponent>().attenuationMod;
			ubo.spotlights[lightIndex].specularMod = actor.getComponent<Components::SpotlightComponent>().specularMod;
			lightIndex += 1;
		}

		});
		ubo.numSpotlights = lightIndex;
	}

	void SpotlightSystem::render(FrameInfo& frameInfo, std::shared_ptr<wb3d::Scene>& scene) {
		scene->eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, scene.get() };
		if (!actor) return;
		// copy light to ubo
		if (actor.hasComponent<Components::SpotlightComponent>()) {
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

			if (actor.getComponent<Components::SpotlightComponent>().attenuationMod != glm::vec4(0.f, 0.f, 1.f, 0.f) && ini.GetBoolValue("WARNINGS", "warn.NotInverseSquareAttenuation")) {
				std::cout << "warn.NotInverseSquareAttenuation: \"Spotlight in level does not obey inverse square law\"\n";
			}
			if (actor.getComponent<Components::SpotlightComponent>().outerAngle > actor.getComponent<Components::SpotlightComponent>().innerAngle && ini.GetBoolValue("WARNINGS", "warn.InvertedSpotlightAngle")) {
				std::cout << "warn.InvertedSpotlightAngle: \"Spotlight in level that has inner angle greater than outer angle, spotlight won't render correctly\"\n";
			}

			SpotlightPushConstants push{};
			push.position = glm::vec4(actor.getComponent<Components::TransformComponent>().translation, 1.f);
			push.color = glm::vec4(actor.getComponent<Components::SpotlightComponent>().color, actor.getComponent<Components::SpotlightComponent>().lightIntensity);
			push.direction = glm::vec4(actor.getComponent<Components::TransformComponent>().rotation, 1.f);
			push.angle = glm::vec2(actor.getComponent<Components::SpotlightComponent>().outerAngle, actor.getComponent<Components::SpotlightComponent>().innerAngle);
			push.radius = actor.getComponent<Components::TransformComponent>().scale.x / 10;
			push.attenuationMod = actor.getComponent<Components::SpotlightComponent>().attenuationMod;
			push.specularMod = actor.getComponent<Components::SpotlightComponent>().specularMod;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SpotlightPushConstants),
				&push
			);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
		});
	}

}