#include "../../s3dpch.h" 

#include "shadow_mapping_system.h"
#include "../../core/vulkan_api/pipeline.h"
#include "../../core/vulkan_api/descriptors.h"
#include "../../ecs.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Shard3D {
	struct ShadowPushConstants {
		glm::mat4 projection;
		glm::mat4 model;
	};


	ShadowMappingSystem::ShadowMappingSystem(EngineDevice& device) : engineDevice(device) {
		lightCamera.setOrthographicProjection(-35.f, 35.f, 35.f, -35.f, 0.1f, 75.f);
		
		createRenderPass();
		createPipelineLayout();
		createPipeline();
	}
	ShadowMappingSystem::~ShadowMappingSystem() {
		delete shadowFrameBuffer;
		delete shadowRenderpass;
		delete shadowDepthFramebufferAttachment;
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void ShadowMappingSystem::createRenderPass() {			
		shadowDepthFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			glm::ivec3(1024, 1024, 1),
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_SAMPLE_COUNT_1_BIT
			}, FrameBufferAttachmentType::Depth);

		AttachmentInfo depthAttachmentInfo{};
		depthAttachmentInfo.frameBufferAttachment = shadowDepthFramebufferAttachment;
		depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		shadowRenderpass = new RenderPass(
			engineDevice, {
			depthAttachmentInfo
			});

		shadowFrameBuffer = new FrameBuffer(engineDevice, shadowRenderpass->getRenderPass(), { shadowDepthFramebufferAttachment });
	}
	void ShadowMappingSystem::createPipelineLayout() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ShadowPushConstants);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}
	void ShadowMappingSystem::createPipeline() {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		GraphicsPipelineConfigInfo pipelineConfig{};
		GraphicsPipeline::pipelineConfig(pipelineConfig)
			.defaultGraphicsPipelineConfigInfo()
			.enableVertexDescriptions()
			.forceSampleCount(VK_SAMPLE_COUNT_1_BIT);

		pipelineConfig.renderPass = shadowRenderpass->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout; // support only view plane aligned atm
		graphicsPipeline = make_uPtr<GraphicsPipeline>(
			engineDevice,
			"assets/shaderdata/shadow_directional.vert.spv",
			"assets/shaderdata/fragment_blank.frag.spv",
			pipelineConfig
		);
	}

	void ShadowMappingSystem::render(FrameInfo& frameInfo) {
		shadowRenderpass->beginRenderPass(frameInfo, shadowFrameBuffer);
		graphicsPipeline->bind(frameInfo.commandBuffer);
		ShadowPushConstants push{};

		auto viewCam = frameInfo.activeLevel->registry.view<Components::DirectionalLightComponent, Components::TransformComponent>();
		for (auto obj : viewCam) {
			ECS::Actor actor = { obj, frameInfo.activeLevel.get() };
			auto& transform = actor.getTransform();
			auto& component = actor.getComponent<Components::DirectionalLightComponent>();
			lightCamera.setViewDirection({0.f, 10.f, 0.f}, transform.getRotation(), glm::vec3(0.f, 1.f, 0.f));
			component.lightProjection = push.projection = lightCamera.getProjection() * lightCamera.getView();

			auto view = frameInfo.activeLevel->registry.view<Components::MeshComponent, Components::TransformComponent>();
			for (auto obj : view) {
				ECS::Actor actor = { obj, frameInfo.activeLevel.get() };
				auto& transform = actor.getTransform();
				auto& component = actor.getComponent<Components::MeshComponent>();

				push.model = transform.transformMatrix;

				auto& model = ResourceHandler::retrieveMesh(component.asset);

				vkCmdPushConstants(
					frameInfo.commandBuffer,
					pipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT,
					0,
					sizeof(ShadowPushConstants),
					&push
				);

				for (int i = 0; i < model->bind(frameInfo.commandBuffer); i++) {
					model->draw(frameInfo.commandBuffer, i);
				}
			}
		}

		
		shadowRenderpass->endRenderPass(frameInfo);
	}
}