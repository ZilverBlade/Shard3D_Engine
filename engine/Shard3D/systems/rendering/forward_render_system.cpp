#pragma deprecated

#include <glm/gtc/constants.hpp>

#include "forward_render_system.h"

#include "../handlers/material_handler.h"
#include "../handlers/render_handler.h"
#include "../../core/asset/assetmgr.h"
#include "../../core/asset/cubemap.h"
#include "../../ecs.h"

namespace Shard3D::Systems {
#if 0
	struct MeshPushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	ForwardRenderSystem::ForwardRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		MaterialHandler::setCurrentDevice(device);
		MaterialHandler::setRenderPassContext(renderPass);
		MaterialHandler::setGlobalSetLayout(globalSetLayout);

		MaterialHandler::setAllAvailableMaterialShaderPermutations({
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Opaque,
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Masked,
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent,
			SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent | SurfaceMaterialClassOptions_Masked
		});
	}
	ForwardRenderSystem::~ForwardRenderSystem() {}

	void ForwardRenderSystem::renderForward(FrameInfo& frameInfo) {
		auto view = frameInfo.activeLevel->registry.view<Components::Mesh3DComponent, Components::TransformComponent>();
		for (auto obj : view) { ECS::Actor actor = { obj, frameInfo.activeLevel.get() };
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
				material->bind(frameInfo.commandBuffer, frameInfo.globalDescriptorSet);
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
	}

	void ForwardRenderSystem::renderForwardOld(FrameInfo& frameInfo) {
		hashMap<uint32_t, std::vector<uint32_t>> deferredTransparency;
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
					deferredTransparency[(uint32_t)obj].push_back(i);
					continue;
				}
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
				MaterialHandler::getMaterialClass(material->getClass())->bind(frameInfo.commandBuffer);
				material->bind(frameInfo.commandBuffer, frameInfo.globalDescriptorSet);
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
		for (auto& [actor_, indices] : deferredTransparency) {
			ECS::Actor actor = { (entt::entity)actor_, frameInfo.activeLevel.get() };
			auto& transform = actor.getTransform();
			auto& component = actor.getComponent<Components::Mesh3DComponent>();
			MeshPushConstantData push{};
			push.modelMatrix = transform.transformMatrix;
			push.normalMatrix = transform.normalMatrix;

			auto& model = ResourceHandler::retrieveMesh(component.asset);
			SHARD3D_SILENT_ASSERT(model->materialSlots.size() == component.materials.size() && "Mesh Component and 3D model do not match!");
			SHARD3D_ASSERT(model->materialSlots.size() <= component.materials.size() && "Mesh Component has less slots than 3D model!");

			model->bind(frameInfo.commandBuffer);
			for (uint32_t index : indices) {
				rPtr<SurfaceMaterial> material = ResourceHandler::retrieveSurfaceMaterial(component.materials[index]);
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
				MaterialHandler::getMaterialClass(material->getClass())->bind(frameInfo.commandBuffer);
				material->bind(frameInfo.commandBuffer, frameInfo.globalDescriptorSet);
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

	void ForwardRenderSystem::renderForwardNew(FrameInfo& frameInfo) {
		VkCommandBuffer& commandBuffer = frameInfo.commandBuffer;
		VkDescriptorSet& globalDescriptorSet = frameInfo.globalDescriptorSet;

		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Opaque);
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Masked);

		// transparency
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent | SurfaceMaterialClassOptions_Masked);
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent);
	}

	void ForwardRenderSystem::renderClass(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags) {
		_0_renderClass(commandBuffer, globalDescriptorSet, flags | SurfaceMaterialClassOptions_FrontfaceCulling);
		_0_renderClass(commandBuffer, globalDescriptorSet, flags | SurfaceMaterialClassOptions_NoCulling);
	}

	void ForwardRenderSystem::_0_renderClass(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags) {
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
				ResourceHandler::retrieveSurfaceMaterial(object.mesh->materials[index])->bind(commandBuffer, globalDescriptorSet);
				model->draw(commandBuffer, index);
			}
		}
	}
#endif
}