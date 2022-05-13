#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "pointlight_system.hpp"
#include <stdexcept>
#include <array>

namespace shard {

	struct PointlightPushConstants {
		glm::vec4 position{};
		glm::vec4 color{};
		//glm::vec4 attenuationMod{};
		float radius;
	};

	PointlightSystem::PointlightSystem(ShardDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : shardDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	PointlightSystem::~PointlightSystem() {
		vkDestroyPipelineLayout(shardDevice.device(), pipelineLayout, nullptr);
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
		if (vkCreatePipelineLayout(shardDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}



	void PointlightSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		ShardPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		shardPipeline = std::make_unique<ShardPipeline>(
			shardDevice,
			"shaders/pointlight.vert.spv",
			"shaders/pointlight.frag.spv",
			pipelineConfig
			);
	}

	void PointlightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
		int lightIndex = 0;
		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (obj.pointlight == nullptr) continue;

			// copy light to ubo
			ubo.pointlights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
			ubo.pointlights[lightIndex].color = glm::vec4(obj.color, obj.pointlight->lightIntensity);
			lightIndex += 1;
		}
		ubo.numLights = lightIndex;
	}

	void PointlightSystem::render(FrameInfo &frameInfo) {
		shardPipeline->bind(frameInfo.commandBuffer);

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

		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (obj.pointlight == nullptr) continue;

			PointlightPushConstants push{};
			push.position = glm::vec4(obj.transform.translation, 1.f);
			push.color = glm::vec4(obj.color, obj.pointlight->lightIntensity);
			push.radius = obj.transform.scale.x;

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
	}
}