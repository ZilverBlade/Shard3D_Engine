#include "deferred_render_system.h"

#include "../../s3dpch.h" 

#include <glm/gtc/constants.hpp>

#include "../handlers/material_handler.h"
#include "../handlers/render_handler.h"
#include "../../core/asset/assetmgr.h"
#include "../../core/asset/cubemap.h"
#include "../../ecs.h"
#include "../../core/misc/engine_settings.h"
#include "../../core/rendering/frame_buffer.h"
#include "../../core/rendering/render_pass.h"

namespace Shard3D::Systems {
	struct MeshPushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};


	DeferredRenderSystem::DeferredRenderSystem(EngineDevice& device, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		MaterialHandler::setCurrentDevice(device);
		MaterialHandler::setGlobalSetLayout(globalSetLayout);
		MaterialHandler::setDeferredRenderingSystem(this);

		createRenderPass();
		createFramebuffer();

		subpassInputLayout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorImageInfo dbufferimageInfo = depthFramebufferAttachment->getDescriptor();
		VkDescriptorImageInfo normrimageInfo = normalFramebufferAttachment->getDescriptor();
		VkDescriptorImageInfo mdiffuseimageInfo = materialDiffuseFramebufferAttachment->getDescriptor();
		VkDescriptorImageInfo mparamimageInfo = materialParamFramebufferAttachment->getDescriptor();

		EngineDescriptorWriter(*subpassInputLayout, *SharedPools::staticMaterialPool)
			.writeImage(0, &dbufferimageInfo)
			.writeImage(1, &normrimageInfo)
			.writeImage(2, &mdiffuseimageInfo)
			.writeImage(3, &mparamimageInfo)
			.build(subpassInputDescriptorSet);

		MaterialHandler::setRenderPassContext(deferredRenderpass->getRenderPass());
		MaterialHandler::setAllAvailableMaterialShaderPermutations({
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Opaque,
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Masked,
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent,
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent | SurfaceMaterialClassOptions_Masked
			});
	}
	DeferredRenderSystem::~DeferredRenderSystem() {
		delete deferredFrameBuffer;
		delete deferredRenderpass;

		delete finalFramebufferAttachment;
		delete depthFramebufferAttachment;

		delete normalFramebufferAttachment;
		delete materialDiffuseFramebufferAttachment;
		delete materialParamFramebufferAttachment;

		MaterialHandler::destroy();
	}

	void DeferredRenderSystem::createRenderPass() {
		auto framebufferFormat = EngineSettings::getStaticApplicationInfo().highPrecisionFrameBuffer ? VK_FORMAT_R32G32B32A32_SFLOAT : VK_FORMAT_R16G16B16A16_SFLOAT;
		auto depthFormat = EngineSettings::getStaticApplicationInfo().highPrecisionDepthBuffer ? engineDevice.findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) : VK_FORMAT_D24_UNORM_S8_UINT;
		glm::ivec3 windowSize = { 1280, 720, 1 };
		finalFramebufferAttachment = new Rendering::FrameBufferAttachment(engineDevice, {
			framebufferFormat,
			VK_IMAGE_ASPECT_COLOR_BIT,
			windowSize,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_SAMPLE_COUNT_1_BIT
			}, FrameBufferAttachmentType::Color
		);

		depthFramebufferAttachment = new Rendering::FrameBufferAttachment(engineDevice, {
			depthFormat,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			windowSize,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_SAMPLE_COUNT_1_BIT
			}, FrameBufferAttachmentType::Depth
		);

		normalFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
			framebufferFormat,
			VK_IMAGE_ASPECT_COLOR_BIT,
			windowSize,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_SAMPLE_COUNT_1_BIT
			}, FrameBufferAttachmentType::Color
		);

		materialDiffuseFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
			framebufferFormat,
			VK_IMAGE_ASPECT_COLOR_BIT,
			windowSize,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_SAMPLE_COUNT_1_BIT
			}, FrameBufferAttachmentType::Color
		);

		materialParamFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
			framebufferFormat,
			VK_IMAGE_ASPECT_COLOR_BIT,
			windowSize,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_SAMPLE_COUNT_1_BIT
			}, FrameBufferAttachmentType::Color
		);

		deferredRenderpass = new RenderPassAdvanced(
			engineDevice, {
			{.frameBufferAttachment = finalFramebufferAttachment, .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE },
			{.frameBufferAttachment = depthFramebufferAttachment, .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE },
			{.frameBufferAttachment = normalFramebufferAttachment, .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE },
			{.frameBufferAttachment = materialDiffuseFramebufferAttachment, .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE },
			{.frameBufferAttachment = materialParamFramebufferAttachment, .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE }
			}, {
				{.renderTargets = { 1, 2, 3, 4 }, .subpassInputs = {} },
				{.renderTargets = { 0 }, .subpassInputs = { 1, 2, 3, 4 } }  // forward has depth test disabled for orderless transparency, thus needs the position framebuffer. This is also the same subpass used for billboards
			});
	}

	void DeferredRenderSystem::createFramebuffer() {
		deferredFrameBuffer = new FrameBuffer(engineDevice, deferredRenderpass->getRenderPass(), {
				finalFramebufferAttachment,
				depthFramebufferAttachment,
				normalFramebufferAttachment,
				materialDiffuseFramebufferAttachment,
				materialParamFramebufferAttachment
			}
		);
	}

	void DeferredRenderSystem::resize(glm::ivec3 newSize) {
		deferredFrameBuffer->resize(newSize, deferredRenderpass->getRenderPass());

		VkDescriptorImageInfo dbufferimageInfo = depthFramebufferAttachment->getDescriptor();
		VkDescriptorImageInfo normrimageInfo = normalFramebufferAttachment->getDescriptor();
		VkDescriptorImageInfo mdiffuseimageInfo = materialDiffuseFramebufferAttachment->getDescriptor();
		VkDescriptorImageInfo mparamimageInfo = materialParamFramebufferAttachment->getDescriptor();

		EngineDescriptorWriter(*subpassInputLayout, *SharedPools::staticMaterialPool)
			.writeImage(0, &dbufferimageInfo)
			.writeImage(1, &normrimageInfo)
			.writeImage(2, &mdiffuseimageInfo)
			.writeImage(3, &mparamimageInfo)
			.build(subpassInputDescriptorSet);
	}
	void DeferredRenderSystem::renderDeferred(FrameInfo& frameInfo) {
		auto view = frameInfo.activeLevel->registry.view<Components::Mesh3DComponent, Components::TransformComponent>();
		for (auto obj : view) {
			ECS::Actor actor = { obj, frameInfo.activeLevel.get() };
			auto& transform = actor.getTransform();
			auto& component = actor.getComponent<Components::Mesh3DComponent>();
			MeshPushConstantData push{};
			push.modelMatrix = transform.transformMatrix;
			push.normalMatrix = transform.normalMatrix;

			auto& model = ResourceHandler::retrieveMesh(component.asset);
			SHARD3D_SILENT_ASSERT(model->materialSlots.size() == component.materials.size() && "Mesh Component and 3D model do not match!");
			SHARD3D_ASSERT(model->materialSlots.size() <= component.materials.size() && "Mesh Component has less slots than 3D model!");
			for (int i = 0; i < model->bind(frameInfo.commandBuffer); i++) {
				rPtr<SurfaceMaterial> material = ResourceHandler::retrieveSurfaceMaterial(component.materials[i]);
				if (material->getClass() & SurfaceMaterialClassOptions_Translucent) {
					continue;
				}		
				MaterialHandler::bindMaterialClassDeferred(material->getClass(), frameInfo.commandBuffer);
				vkCmdBindDescriptorSets(
					frameInfo.commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					MaterialHandler::getMaterialClass(material->getClass())->getPipelineLayout(),
					0,
					1,
					&frameInfo.globalDescriptorSet,
					0,
					nullptr
				);
				material->bind(frameInfo.commandBuffer);
				vkCmdPushConstants(
					frameInfo.commandBuffer,
					MaterialHandler::getMaterialClass(material->getClass())->getPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT,
					0,
					sizeof(MeshPushConstantData),
					&push
				);
				model->draw(frameInfo.commandBuffer, i);
			}
		}
		deferredRenderpass->nextSubpass(frameInfo.commandBuffer);
		// lighting
		
		MaterialHandler::bindMaterialClassDeferredLighting(frameInfo.commandBuffer, frameInfo.globalDescriptorSet);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);

	}

	void DeferredRenderSystem::renderForward(FrameInfo& frameInfo) {
		// forward
		auto view = frameInfo.activeLevel->registry.view<Components::Mesh3DComponent, Components::TransformComponent>();
		for (auto obj : view) {
			ECS::Actor actor = { obj, frameInfo.activeLevel.get() };
			auto& transform = actor.getTransform();
			auto& component = actor.getComponent<Components::Mesh3DComponent>();
			MeshPushConstantData push{};
			push.modelMatrix = transform.transformMatrix;
			push.normalMatrix = transform.normalMatrix;

			auto& model = ResourceHandler::retrieveMesh(component.asset);
			SHARD3D_SILENT_ASSERT(model->materialSlots.size() == component.materials.size() && "Mesh Component and 3D model do not match!");
			SHARD3D_ASSERT(model->materialSlots.size() <= component.materials.size() && "Mesh Component has less slots than 3D model!");

			model->bind(frameInfo.commandBuffer);
			for (uint32_t index = 0; index < model->bind(frameInfo.commandBuffer); index++) {
				rPtr<SurfaceMaterial> material = ResourceHandler::retrieveSurfaceMaterial(component.materials[index]);
				if (!(material->getClass() & SurfaceMaterialClassOptions_Translucent)) {
					continue;
				}
				VkDescriptorSet sets[2]{
					frameInfo.globalDescriptorSet,
					subpassInputDescriptorSet
				};
				MaterialHandler::bindMaterialClassForward(material->getClass(), frameInfo.commandBuffer);
				vkCmdBindDescriptorSets(
					frameInfo.commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					MaterialHandler::getMaterialClass(material->getClass())->getPipelineLayout(),
					0,
					2,
					sets,
					0,
					nullptr
				);
				material->bind(frameInfo.commandBuffer);
				vkCmdPushConstants(
					frameInfo.commandBuffer,
					MaterialHandler::getMaterialClass(material->getClass())->getPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT,
					0,
					sizeof(MeshPushConstantData),
					&push
				);
				model->draw(frameInfo.commandBuffer, index);
			}
		}
	}

	void DeferredRenderSystem::renderDeferredNew(FrameInfo& frameInfo) {
		// opaque/masked
		for (SurfaceMaterialClassOptionsFlags class_ : MaterialHandler::getDeferrableClasses()) {
			renderClassDeferred(frameInfo.commandBuffer, frameInfo.globalDescriptorSet, class_);
		}
		deferredRenderpass->nextSubpass(frameInfo.commandBuffer);

		// lighting here

		MaterialHandler::bindMaterialClassDeferredLighting(frameInfo.commandBuffer, frameInfo.globalDescriptorSet);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}

	void DeferredRenderSystem::renderForwardNew(FrameInfo& frameInfo) {
		// translucent
		for (SurfaceMaterialClassOptionsFlags class_ : MaterialHandler::getForwardOnlyClasses()) {
			renderClassForward(frameInfo.commandBuffer, frameInfo.globalDescriptorSet, class_);
		}
	}

	void DeferredRenderSystem::beginRenderPass(FrameInfo& frameInfo) {
		deferredRenderpass->beginRenderPass(frameInfo.commandBuffer, deferredFrameBuffer);
	}

	void DeferredRenderSystem::endRenderPass(FrameInfo& frameInfo) {
		deferredRenderpass->endRenderPass(frameInfo.commandBuffer);
	}

	void DeferredRenderSystem::renderClassDeferred(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags) {
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			MaterialHandler::getMaterialClass(flags)->getPipelineLayout(),
			0,
			1,
			&globalDescriptorSet,
			0,
			nullptr
		);
		MaterialHandler::bindMaterialClassDeferred(flags, commandBuffer);
	
		for (auto& [asset, object] : RenderHandler::getSurfaceMaterialRenderingList()[flags]) {
			SHARD3D_ASSERT(object.mesh && object.transform && "Cannot render a non existant mesh!");
	
			rPtr<Model3D>& model = ResourceHandler::retrieveMesh(object.mesh->asset);
	
			SHARD3D_SILENT_ASSERT(model->materialSlots.size() == object.mesh->materials.size() && "Mesh Component and 3D model do not match!");
			SHARD3D_ASSERT(model->materialSlots.size() <= object.mesh->materials.size() && "Mesh Component has less slots than 3D model!");
	
			model->bind(commandBuffer);
			MeshPushConstantData push{};
			push.modelMatrix = object.transform->transformMatrix;
			push.normalMatrix = object.transform->normalMatrix;
			vkCmdPushConstants(
				commandBuffer,
				MaterialHandler::getMaterialClass(flags)->getPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(MeshPushConstantData),
				&push
			);
			for (uint32_t index : object.material_indices) {
				ResourceHandler::retrieveSurfaceMaterial(object.mesh->materials[index])->bind(commandBuffer);
				model->draw(commandBuffer, index);
			}
		}
	}
	void DeferredRenderSystem::renderClassForward(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags) {
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			MaterialHandler::getMaterialClass(flags)->getPipelineLayout(),
			0,
			1,
			&globalDescriptorSet,
			0,
			nullptr
		);
		MaterialHandler::bindMaterialClassForward(flags, commandBuffer);
		VkDescriptorSet sets[2]{
			globalDescriptorSet,
			subpassInputDescriptorSet
		};
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			MaterialHandler::getMaterialClass(flags)->getPipelineLayout(),
			0,
			2,
			sets,
			0,
			nullptr
		);
		for (auto& [asset, object] : RenderHandler::getSurfaceMaterialRenderingList()[flags]) {
			SHARD3D_ASSERT(object.mesh && object.transform && "Cannot render a non existant mesh!");

			rPtr<Model3D>& model = ResourceHandler::retrieveMesh(object.mesh->asset);

			SHARD3D_SILENT_ASSERT(model->materialSlots.size() == object.mesh->materials.size() && "Mesh Component and 3D model do not match!");
			SHARD3D_ASSERT(model->materialSlots.size() <= object.mesh->materials.size() && "Mesh Component has less slots than 3D model!");

			model->bind(commandBuffer);
			MeshPushConstantData push{};
			push.modelMatrix = object.transform->transformMatrix;
			push.normalMatrix = object.transform->normalMatrix;
			vkCmdPushConstants(
				commandBuffer,
				MaterialHandler::getMaterialClass(flags)->getPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(MeshPushConstantData),
				&push
			);
			for (uint32_t index : object.material_indices) {
				ResourceHandler::retrieveSurfaceMaterial(object.mesh->materials[index])->bind(commandBuffer);
				model->draw(commandBuffer, index);
			}
		}
	}
}