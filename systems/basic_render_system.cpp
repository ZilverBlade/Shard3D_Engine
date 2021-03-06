#include "../s3dtpch.h" 
#include <glm/gtc/constants.hpp>

#include "basic_render_system.hpp"

#include "../wb3d/assetmgr.hpp"
#include "../cubemap.hpp"
namespace Shard3D {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	BasicRenderSystem::BasicRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
		//std::shared_ptr<EngineCubemap> cubemap = 
		//	EngineCubemap::createCubemapFromFolder(device, "assets/_engine/tex/cubemaps/sky0", VK_FILTER_LINEAR);

		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	BasicRenderSystem::~BasicRenderSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}
	
	void BasicRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		
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

	void BasicRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
		EnginePipeline::enableVertexDescriptions(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		CSimpleIniA ini;
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		ini.SetUnicode();
		if (ini.GetBoolValue("RENDERING", "PBR"))
			enginePipeline = std::make_unique<EnginePipeline>(
				engineDevice,
				"assets/shaderdata/pbr_shader.vert.spv",
				"assets/shaderdata/pbr_shader.frag.spv",
				pipelineConfig
			);
		else 
			enginePipeline = std::make_unique<EnginePipeline>(
				engineDevice,
				"assets/shaderdata/basic_shader.vert.spv",
				"assets/shaderdata/basic_shader.frag.spv",
				pipelineConfig
			);
	}

	void BasicRenderSystem::renderGameObjects(FrameInfo& frameInfo, std::shared_ptr<wb3d::Level>& level) {
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

		level->registry.view<Components::MeshComponent, Components::TransformComponent>().each([&](auto mesh, auto transform){
			SimplePushConstantData push{};
			push.modelMatrix = transform.mat4();
			push.normalMatrix = transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);

			auto& model = wb3d::AssetManager::retrieveMesh(mesh.file);
			model->bind(frameInfo.commandBuffer);
			model->draw(frameInfo.commandBuffer);	
		});
	}

}