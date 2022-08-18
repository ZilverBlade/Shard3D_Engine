#include "material.h"
#include "assetmgr.h"

namespace Shard3D {

#pragma region Surface
	void SurfaceMaterial::bind(VkCommandBuffer commandBuffer) {
		SHARD3D_ASSERT(materialPipelineConfig != nullptr, "Cannot bind pipeline to surface material that has not been created!");

		// TODO: change this to cache certain pipelines to speed up binding
		// Or, change this that the model.cpp file submits the primitives to some sort of unordered map to render, categorised by the pipelines,
		// however this wont work when custom material shaders files can be created.
		materialPipelineConfig->shaderPipeline->bind(commandBuffer);	

		VkDescriptorSet sets[] = {
			materialDescriptorInfo.factors,
			materialDescriptorInfo.textures
		};

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			materialPipelineConfig->pipelineLayout,
			0,
			2,
			sets,
			0,
			nullptr
		);

		vkCmdSetCullMode(commandBuffer, drawData.culling);
	}
#pragma region helper functions

	struct MeshPushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	static inline void createPipeline(
		EngineDevice& device,
		VkRenderPass renderPass,
		VkDescriptorSetLayout globalSetLayout,
		MaterialPipelineConfigInfo* materialConfig,
		PipelineConfigInfo& pipelineConfig,
		const std::string& fragment_shader
	) {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			globalSetLayout
		};

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(MeshPushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &materialConfig->pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = materialConfig->pipelineLayout;

		materialConfig->shaderPipeline = make_uPtr<EnginePipeline>(
			device,
			"assets/shaderdata/mesh_shader.vert.spv",
			fragment_shader,
			pipelineConfig
		);
	}

#pragma endregion
#pragma region SurfaceShaded

	void SurfaceMaterial_ShadedOpaque::setupMaterialShaderPipeline(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
		PipelineConfigInfo pipelineConfigInfo{};
		EnginePipeline::pipelineConfig(pipelineConfigInfo)
			.defaultPipelineConfigInfo()
			.enableVertexDescriptions();
		createPipeline(device, renderPass, globalSetLayout, &surfaceShadedOpaque, pipelineConfigInfo, "assets/shaderdata/materials/surface_shaded_opaque.frag.spv");
	}
	void SurfaceMaterial_ShadedOpaque::createMaterialDescriptors(uPtr<EngineDescriptorPool>& descriptorPool) {
		VkDescriptorImageInfo diffuseTex_imageInfo = AssetManager::retrieveTexture(diffuseTex)->getImageInfo();
		VkDescriptorImageInfo specularTex_imageInfo = AssetManager::retrieveTexture(specularTex)->getImageInfo();
		VkDescriptorImageInfo shininessTex_imageInfo = AssetManager::retrieveTexture(shininessTex)->getImageInfo();
		VkDescriptorImageInfo metallicTex_imageInfo = AssetManager::retrieveTexture(metallicTex)->getImageInfo();

		VkDescriptorImageInfo normalTex_imageInfo = AssetManager::retrieveTexture(normalTex)->getImageInfo();

		EngineDescriptorWriter(*materialDescriptorInfo.imageLayout, *descriptorPool)
				.writeImage(1, &diffuseTex_imageInfo)
				.writeImage(2, &specularTex_imageInfo)
				.writeImage(3, &shininessTex_imageInfo)
				.writeImage(4, &metallicTex_imageInfo)
				.writeImage(5, &normalTex_imageInfo)
				.build(materialDescriptorInfo.textures);

		materialPipelineConfig = &surfaceShadedOpaque;
	}
	
#pragma endregion
#pragma region SurfaceUnshaded


#pragma endregion
#pragma endregion


}