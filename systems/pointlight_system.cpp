#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "pointlight_system.hpp"
#include <stdexcept>
#include <array>
#include <map>

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
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}



	void PointlightSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		const char* vertFile = "pointlight.vert.spv";
		const char* fragFile = "pointlight.frag.spv";

		char* vertShader = (char*)(calloc(strlen(SHADER_FILES_PATH) + strlen(vertFile) - 1, 1));
		strncpy(vertShader, SHADER_FILES_PATH, strlen(SHADER_FILES_PATH));
		strncat(vertShader, vertFile, strlen(vertFile));

		char* fragShader = (char*)(calloc(strlen(SHADER_FILES_PATH) + strlen(fragFile) - 1, 1));
		strncpy(fragShader, SHADER_FILES_PATH, strlen(SHADER_FILES_PATH));
		strncat(fragShader, fragFile, strlen(fragFile));

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
		EnginePipeline::enableAlphaBlending(pipelineConfig, VK_BLEND_OP_ADD);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			vertShader,
			fragShader,
			pipelineConfig
		);
	}

	void PointlightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
		int lightIndex = 0;
		level->eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, level.get() };
			if (!actor) return;

			if (actor.hasComponent<Components::PointlightComponent>()) {
				ubo.pointlights[lightIndex].position = glm::vec4(actor.getComponent<Components::TransformComponent>().translation, 1.f);
				ubo.pointlights[lightIndex].color = glm::vec4(actor.getComponent<Components::PointlightComponent>().color, actor.getComponent<Components::PointlightComponent>().lightIntensity);
				ubo.pointlights[lightIndex].attenuationMod = glm::vec4(actor.getComponent<Components::PointlightComponent>().attenuationMod, 0.f);
				ubo.pointlights[lightIndex].specularMod = actor.getComponent<Components::PointlightComponent>().specularMod;
				lightIndex += 1;
			}
		});
		ubo.numPointlights = lightIndex;
	}

	void PointlightSystem::render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Level>& level) {
		enginePipeline->bind(frameInfo.commandBuffer);

		level->eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, level.get() };
			if (!actor) return;
			// copy light to ubo
			if (actor.hasComponent<Components::PointlightComponent>()) {
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

				if (actor.getComponent<Components::PointlightComponent>().attenuationMod != glm::vec3(0.f, 0.f, 1.f) && ini.GetBoolValue("WARNINGS", "warn.NotInverseSquareAttenuation")) {
					std::cout << "warn.NotInverseSquareAttenuation: \"Pointlight in level does not obey inverse square law\"\n";
				}

				PointlightPushConstants push{};
				push.position = glm::vec4(actor.getComponent<Components::TransformComponent>().translation, 1.f);
				push.color = glm::vec4(actor.getComponent<Components::PointlightComponent>().color, actor.getComponent<Components::PointlightComponent>().lightIntensity);
				push.radius = actor.getComponent<Components::PointlightComponent>().radius / 10;
				push.attenuationMod = glm::vec4(actor.getComponent<Components::PointlightComponent>().attenuationMod, 0.f);
				push.specularMod = actor.getComponent<Components::PointlightComponent>().specularMod;

				vkCmdPushConstants(
					frameInfo.commandBuffer,
					pipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(PointlightPushConstants),
					&push
				);
				vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
			}
		});
	}
}