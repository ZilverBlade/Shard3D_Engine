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
		MaterialSystem::setRenderPassContext(renderPass);
		MaterialSystem::setGlobalSetLayout(globalSetLayout);
		//std::shared_ptr<EngineCubemap> cubemap = 
		//	EngineCubemap::createCubemapFromFolder(device, "assets/_engine/tex/cubemaps/sky0", VK_FILTER_LINEAR);

		SurfaceMaterial_ShadedOpaque::setupMaterialShaderPipeline(device, renderPass, globalSetLayout);
	}
	ForwardRenderSystem::~ForwardRenderSystem() {
		
	}


	void ForwardRenderSystem::renderForward(FrameInfo& frameInfo) {;

		
		//auto imageInfo = wb3d::AssetManager::retrieveTexture("assets/_engine/tex/cubemaps/sky0/yes.png")->getImageInfo();
		//VkDescriptorSet descriptorSet1;
		//EngineDescriptorWriter(*skyboxLayout, frameInfo.perDrawDescriptorPool)
		//	.writeImage(20, &imageInfo)
		//	.build(descriptorSet1);
		//
		//vkCmdBindDescriptorSets(
		//	frameInfo.commandBuffer,
		//	VK_PIPELINE_BIND_POINT_GRAPHICS,
		//	pipelineLayout,
		//	1,  // first set
		//	1,  // set count
		//	&descriptorSet1,
		//	0,
		//	nullptr);

		auto view = frameInfo.activeLevel->registry.view<Components::MeshComponent, Components::TransformComponent>();
		for (auto obj : view) { ECS::Actor actor = { obj, frameInfo.activeLevel.get() };
			auto& transform = actor.getTransform();
			MeshPushConstantData push{};
			push.modelMatrix = frameInfo.activeLevel->getParentMat4(actor) * transform.mat4() ;
			push.normalMatrix = frameInfo.activeLevel->getParentNormals(actor) * transform.normalMatrix();

			auto& model = AssetManager::retrieveMesh(actor.getComponent<Components::MeshComponent>().file);
			for (auto& buffer : model->buffers) {
				model->bindMaterial(frameInfo.commandBuffer, frameInfo.globalDescriptorSet, buffer);		
				vkCmdPushConstants(
					frameInfo.commandBuffer,
					model->getMaterialPipelineLayout(buffer),
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(MeshPushConstantData),
					&push
				);
				model->bind(frameInfo.commandBuffer, buffer);
				model->draw(frameInfo.commandBuffer, buffer);
			}
		}

		/*level->registry.each([&](auto actorGUID) { Actor actor = { actorGUID, level.get() };
			auto& transform = actor.getTransform();
			SimplePushConstantData push{};
			push.modelMatrix = level->getParentMat4(actor) * transform.mat4();
			push.normalMatrix = transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);

			auto& model = wb3d::AssetManager::retrieveMesh(actor.getComponent<Components::MeshComponent>().file);
			model->bind(frameInfo.commandBuffer);
			model->draw(frameInfo.commandBuffer);
		});*/
	}

}