#include "../../s3dpch.h" 
#include <glm/gtc/constants.hpp>
#include <glm/gtx/dual_quaternion.hpp>

#include "hud_system.h"

#include "../../core/asset/assetmgr.h"
#include "../../core/misc/graphics_settings.h"

namespace Shard3D {
	struct HUDPushConstants {
		glm::vec4 PZR;
		glm::vec2 scale;
		glm::vec2 mousePos;
		glm::vec2 anchorOffset;
		float ar;

		bool activeElement;
		uint64_t id;
	};

	struct HUDInfo {
		uint64_t selectedID = 0;
	};

	HUDRenderSystem::HUDRenderSystem() {}
	HUDRenderSystem::~HUDRenderSystem() {}
	void HUDRenderSystem::reset() {
		pickBuffer->writeToBuffer(new HUDInfo());
		pickBuffer->flush();
	}
	void HUDRenderSystem::create(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass) {
		device = &dvc;
		window = &wnd;
		pickBuffer = make_uPtr<EngineBuffer>(
			dvc,
			sizeof(HUDInfo),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
		pickBuffer->map();
		pickBuffer->writeToBuffer(new HUDInfo());
		pickBuffer->flush();

		createPipelineLayout();
		createPipeline(renderPass);
	}
	void HUDRenderSystem::destroy() {
		vkDestroyPipelineLayout(device->device(), pipelineLayout, nullptr);
	}
	uint64_t HUDRenderSystem::getSelectedID() {
		auto* v = pickBuffer->getMappedMemory();
		auto u = static_cast<uint64_t*>(v);
		return *u;
	}
	void HUDRenderSystem::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(HUDPushConstants);

		guiSystemLayout =
			EngineDescriptorSetLayout::Builder(*device)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();
		ssboLayout = EngineDescriptorSetLayout::Builder(*device)
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
		if (vkCreatePipelineLayout(device->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}

	void HUDRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::pipelineConfig(pipelineConfig)
			.defaultPipelineConfigInfo()
			.forceSampleCount(VK_SAMPLE_COUNT_1_BIT)
			.enableAlphaBlending(VK_BLEND_OP_ADD);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout; 
		enginePipeline = make_uPtr<EnginePipeline>(
			*device,
			"assets/shaderdata/hud_element.vert.spv",
			"assets/shaderdata/hud_element.frag.spv",
			pipelineConfig
		);
	}

	void HUDRenderSystem::render(FrameInfo& frameInfo, HUD& gui) {
		glm::vec2 mousePos;
		{	
			double x, y;
			glfwGetCursorPos(window->getGLFWwindow(), &x, &y);
			mousePos = { x,y };
		}
		float ar = GraphicsSettings::getRuntimeInfo().aspectRatio;
		enginePipeline->bind(frameInfo.commandBuffer);
		
		auto bufferInfo = pickBuffer->descriptorInfo();
		EngineDescriptorWriter(*ssboLayout, frameInfo.perDrawDescriptorPool)
			.writeBuffer(10, &bufferInfo)
			.build(ssboDescriptorSet);

		for (auto& elmt : gui.elements) {
			auto& element = elmt.second;
			uint64_t id = getSelectedID();
			bool isPress = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
			if (isPress && id == element->guid)
				element->_design_tex = element->press_texture;
			else if (id == element->guid)
				element->_design_tex = element->hover_texture;
			else
				element->_design_tex = element->default_texture;
			auto imageInfo = AssetManager::retrieveTexture(element->_design_tex)->getImageInfo();
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
			HUDPushConstants push{};
			push.PZR.x = element->position.x;
			push.PZR.y = -element->position.y;
			push.PZR.z = static_cast<float>(element->zPos);
			push.PZR.w = element->rotation;
			push.scale = -element->scale;
			push.ar = ar;
			push.id = element->guid;
			push.mousePos = mousePos;
			push.anchorOffset = element->anchorOffset;
			push.activeElement = element->isActive;
			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(HUDPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}	
	}
}