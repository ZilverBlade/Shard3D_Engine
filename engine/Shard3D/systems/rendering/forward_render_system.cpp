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
	ForwardRenderSystem::~ForwardRenderSystem() {
		
	}

	void ForwardRenderSystem::renderForward(FrameInfo& frameInfo) {;
		auto view = frameInfo.activeLevel->registry.view<Components::MeshComponent, Components::TransformComponent>();
		for (auto obj : view) { ECS::Actor actor = { obj, frameInfo.activeLevel.get() };
			auto& transform = actor.getTransform();
			auto& component = actor.getComponent<Components::MeshComponent>();
			MeshPushConstantData push{};
			push.modelMatrix = transform.transformMatrix;
			push.normalMatrix = transform.normalMatrix;

			auto& model = ResourceHandler::retrieveMesh(component.asset);
			SHARD3D_ASSERT(model->buffers.size() == component.materials.size() && "Mesh Component and 3D model do not match!");
			for (int i = 0; i < model->buffers.size(); i++) {
				auto& buffer = model->buffers[i];
				rPtr<SurfaceMaterial> material = ResourceHandler::retrieveSurfaceMaterial(component.materials[i]);
				material->bind(frameInfo.commandBuffer, frameInfo.globalDescriptorSet);
				vkCmdPushConstants(
					frameInfo.commandBuffer,
					material->getPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(MeshPushConstantData),
					&push
				);
				model->bind(frameInfo.commandBuffer, buffer);
				model->draw(frameInfo.commandBuffer, buffer);
			}
		}
	}

}