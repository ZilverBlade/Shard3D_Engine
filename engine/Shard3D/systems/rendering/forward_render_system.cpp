#include "../../s3dpch.h" 

#include <glm/gtc/constants.hpp>

#include "forward_render_system.h"

#include "../buffers/material_system.h"
#include "../../core/asset/assetmgr.h"
#include "../../core/asset/cubemap.h"
#include "../../ecs.h"
namespace Shard3D {

	struct MeshPushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	ForwardRenderSystem::ForwardRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		MaterialSystem::setCurrentDevice(device);
		MaterialSystem::setRenderPassContext(renderPass);
		MaterialSystem::setGlobalSetLayout(globalSetLayout);
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
				material->bind(frameInfo.commandBuffer, frameInfo.globalDescriptorSet);
				vkCmdPushConstants(
					frameInfo.commandBuffer,
					material->getPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT,
					0,
					sizeof(MeshPushConstantData),
					&push
				);
				model->draw(frameInfo.commandBuffer, i);
			}
		}
	}

	void ForwardRenderSystem::renderForwardNew(FrameInfo& frameInfo) {
		VkCommandBuffer& commandBuffer = frameInfo.commandBuffer;
		VkDescriptorSet& globalDescriptorSet = frameInfo.globalDescriptorSet;
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_FrontfaceCulling | SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Opaque);
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_FrontfaceCulling | SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Masked);
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_NoCulling | SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Opaque);
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_NoCulling | SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Masked);
		
		// transparency
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_FrontfaceCulling | SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent);
		renderClass(commandBuffer, globalDescriptorSet, SurfaceMaterialClassOptions_NoCulling | SurfaceMaterialClassOptions_Shaded | SurfaceMaterialClassOptions_Translucent);
	}

	void ForwardRenderSystem::renderClass(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, uint32_t flags) {
		MaterialSystem::bindMaterialClass(flags, commandBuffer);
		for (auto& [asset, object] : MaterialSystem::getMaterialRenderingList()[flags]) {
			rPtr<Model3D>& model = ResourceHandler::retrieveMesh(object.mesh->asset);
			model->bind(commandBuffer);
			MeshPushConstantData push{};
			push.modelMatrix = object.transform->transformMatrix;
			push.normalMatrix = object.transform->normalMatrix;
			vkCmdPushConstants(
				commandBuffer,
				MaterialSystem::getMaterialClass(flags)->getPipelineLayout(),
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

}