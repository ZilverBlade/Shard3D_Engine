#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "basic_render_system.hpp"
#include <stdexcept>
#include <array>

namespace Shard3D {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	BasicRenderSystem::BasicRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	BasicRenderSystem::~BasicRenderSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void BasicRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

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

	void BasicRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

#if USE_PBR
		const char* vertFile = "pbr_shader.vert.spv";
		const char* fragFile = "pbr_shader.frag.spv";
#else
		const char* vertFile = "basic_shader.vert.spv";
		const char* fragFile = "basic_shader.frag.spv";
#endif
		char* vertShader = (char*)(calloc(strlen(SHADER_FILES_PATH) + strlen(vertFile) - 1, 1));
		strncpy(vertShader, SHADER_FILES_PATH, strlen(SHADER_FILES_PATH));
		strncat(vertShader, vertFile, strlen(vertFile));

		char* fragShader = (char*)(calloc(strlen(SHADER_FILES_PATH) + strlen(fragFile) - 1, 1));
		strncpy(fragShader, SHADER_FILES_PATH, strlen(SHADER_FILES_PATH));
		strncat(fragShader, fragFile, strlen(fragFile));

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			vertShader,
			fragShader,
			pipelineConfig
		);
	}

	void BasicRenderSystem::renderGameObjects(FrameInfo& frameInfo, std::shared_ptr<wb3d::Level>& level) {
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

		level->registry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, level.get() };
			if (!actor) return;

			if (actor.hasComponent<Components::MeshComponent>()) {
				auto transform = actor.getComponent<Components::TransformComponent>();
				SimplePushConstantData push{};
				push.modelMatrix = transform.mat4();
				push.normalMatrix = transform.normalMatrix();

				vkCmdPushConstants(
					frameInfo.commandBuffer,
					pipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(SimplePushConstantData),
					&push
				);

				auto model = actor.getComponent<Components::MeshComponent>().model;
				model->bind(frameInfo.commandBuffer);
				model->draw(frameInfo.commandBuffer);
			}
		});
	}

}