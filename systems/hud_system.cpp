#include "../s3dtpch.h" 
#include <glm/gtc/constants.hpp>

#include "hud_system.hpp"

#include "../wb3d/assetmgr.hpp"
#include "../singleton.hpp"
#include <glm/gtx/dual_quaternion.hpp>

namespace Shard3D {
	enum class AnchorType {
		TOPLEFT		= 0,
		TOPRIGHT	= 1,
		BOTTOMLEFT  = 2,
		BOTTOMRIGHT = 3
	};
	struct GUIPushConstants {
		glm::vec4 PZR;
		glm::vec2 scale;
		glm::vec2 mousePos;
		float ar;
		AnchorType anchorType;
		uint64_t id;
	};

	struct GUIInfo {
		uint64_t selectedID = 0;
	};

	HUDRenderSystem::HUDRenderSystem() {}
	HUDRenderSystem::~HUDRenderSystem() {}
	void HUDRenderSystem::reset() {
		pickBuffer->writeToBuffer(new GUIInfo());
		pickBuffer->flush();
	}
	void HUDRenderSystem::create(VkRenderPass renderPass) {
		pickBuffer = std::make_unique<EngineBuffer>(
			Singleton::engineDevice,
			sizeof(GUIInfo),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
		pickBuffer->map();
		pickBuffer->writeToBuffer(new GUIInfo());
		pickBuffer->flush();

		createPipelineLayout();
		createPipeline(renderPass);
	}
	void HUDRenderSystem::destroy() {
		vkDestroyPipelineLayout(Singleton::engineDevice.device(), pipelineLayout, nullptr);
	}
	void HUDRenderSystem::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(GUIPushConstants);

		guiSystemLayout =
			EngineDescriptorSetLayout::Builder(Singleton::engineDevice)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();
		ssboLayout = EngineDescriptorSetLayout::Builder(Singleton::engineDevice)
			.addBinding(10, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			guiSystemLayout->getDescriptorSetLayout(),
			ssboLayout->getDescriptorSetLayout()
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(Singleton::engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}

	void HUDRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
	 	EnginePipeline::enableAlphaBlending(pipelineConfig, VK_BLEND_OP_ADD);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout; 
		enginePipeline = std::make_unique<EnginePipeline>(
			Singleton::engineDevice,
			"assets/shaderdata/gui_element.vert.spv",
			"assets/shaderdata/gui_element.frag.spv",
			pipelineConfig
		);
	}

	void HUDRenderSystem::render(FrameInfo& frameInfo, HUD& gui) {
		glm::vec2 mousePos;
		{	
			double x, y;
			glfwGetCursorPos(Singleton::engineWindow.getGLFWwindow(), &x, &y);
			mousePos = { x,y };
		}
		float ar = Singleton::engineRenderer.getAspectRatio();
		enginePipeline->bind(frameInfo.commandBuffer);
		
		auto bufferInfo = pickBuffer->descriptorInfo();
		EngineDescriptorWriter(*ssboLayout, frameInfo.perDrawDescriptorPool)
			.writeBuffer(10, &bufferInfo)
			.build(ssboDescriptorSet);

		for (auto& elmt : gui.elements) {
			auto& element = elmt.second;
			auto imageInfo = wb3d::AssetManager::retrieveTexture(element->texturePath)->getImageInfo();
			VkDescriptorSet descriptorSet1;
			EngineDescriptorWriter(*guiSystemLayout, frameInfo.perDrawDescriptorPool)
				.writeImage(2, &imageInfo)
				.build(descriptorSet1);

			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipelineLayout,
				0,  // first set
				1,  // set count
				&descriptorSet1,
				0,
				nullptr);
			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipelineLayout,
				1,  // first set
				1,  // set count
				&ssboDescriptorSet,
				0,
				nullptr);
			GUIPushConstants push{};
			push.PZR.x = element->position.x;
			push.PZR.y = element->position.y;
			push.PZR.z = static_cast<float>(element->zPos);
			push.PZR.w = element->rotation;
			push.scale = -element->scale;
			push.ar = ar;
			push.id = element->guid;
			push.mousePos = mousePos;
			push.anchorType = static_cast<AnchorType>(element->anchorType);
			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(GUIPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}	
	}
}