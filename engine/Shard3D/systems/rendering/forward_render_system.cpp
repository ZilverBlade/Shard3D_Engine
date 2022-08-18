#include "../../s3dpch.h" 

#include <glm/gtc/constants.hpp>

#include "forward_render_system.h"

#include "../../core/asset/assetmgr.h"
#include "../../core/asset/cubemap.h"
#include "../../ecs.h"
namespace Shard3D {

	struct MeshPushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	ForwardRenderSystem::ForwardRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		//std::shared_ptr<EngineCubemap> cubemap = 
		//	EngineCubemap::createCubemapFromFolder(device, "assets/_engine/tex/cubemaps/sky0", VK_FILTER_LINEAR);

		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	ForwardRenderSystem::~ForwardRenderSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void ForwardRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(MeshPushConstantData);

		//skyboxLayout =
		//	EngineDescriptorSetLayout::Builder(engineDevice)
		//	.addBinding(20, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		//	.build();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			globalSetLayout
			//,skyboxLayout->getDescriptorSetLayout()
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}

	void ForwardRenderSystem::createPipeline(VkRenderPass renderPass) {
		SHARD3D_ASSERT(pipelineLayout != nullptr, "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::pipelineConfig(pipelineConfig)
			.defaultPipelineConfigInfo()
			.enableVertexDescriptions();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		CSimpleIniA ini;
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		ini.SetUnicode();
		
		enginePipeline = make_uPtr<EnginePipeline>(
			engineDevice,
			"assets/shaderdata/surface_shader.vert.spv",
			"assets/shaderdata/surface_shader.frag.spv",
			pipelineConfig
		);
	}

	void ForwardRenderSystem::renderForward(FrameInfo& frameInfo) {;
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

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(MeshPushConstantData),
				&push
			);

			auto& model = AssetManager::retrieveMesh(actor.getComponent<Components::MeshComponent>().file);
			for (auto& buffer : model->buffers) {
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