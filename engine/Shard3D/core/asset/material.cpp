#include "material.h"
#include "assetmgr.h"
#include "../../systems/buffers/material_system.h"

namespace Shard3D {

#pragma region Surface
	void SurfaceMaterial::bind(VkCommandBuffer commandBuffer, VkDescriptorSet globalSet) {
		SHARD3D_ASSERT(materialPipelineConfig != nullptr && "Cannot bind pipeline to surface material that has not been created!");
		SHARD3D_ASSERT(materialPipelineConfig->shaderPipeline != nullptr && "Using pipeline before being created!");
		SHARD3D_ASSERT(built && "Material descriptors and pipelines must be built before surface material can be bound!");
		// TODO: change this to cache certain pipelines to speed up binding
		// Or, change this that the model.cpp file submits the primitives to some sort of unordered map to render, categorised by the pipelines,
		// however this wont work when custom material shaders files can be created.
		SHARD3D_LOG("Binding material {0}", materialTag);

		materialPipelineConfig->shaderPipeline->bind(commandBuffer);	

		VkDescriptorSet sets[] = {
			globalSet,
			materialDescriptorInfo.factors,
			materialDescriptorInfo.textures
		};

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			materialPipelineConfig->pipelineLayout,
			0,
			3,
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

#pragma endregion
#pragma region SurfaceShaded
	struct SurfaceMaterial_ShadedOpaque_Factors {
		glm::vec4 diffuse{1.f, 1.f, 1.f, 1.f};
		glm::vec3 SSM{0.f, 0.f, 0.f}; // Specular, Shininess, Metallic
	};

	void SurfaceMaterial_ShadedOpaque::setupMaterialShaderPipeline(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
		}
	void SurfaceMaterial_ShadedOpaque::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
		uPtr<EngineBuffer> factors_buffer =
			make_uPtr<EngineBuffer>(
				device,
				sizeof(SurfaceMaterial_ShadedOpaque_Factors),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

		factors_buffer->map();

		materialDescriptorInfo.factorLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorBufferInfo bufferInfo = factors_buffer->descriptorInfo();
		EngineDescriptorWriter(*materialDescriptorInfo.factorLayout, *SharedPools::staticMaterialPool)
			.writeBuffer(1, &bufferInfo)
			.build(materialDescriptorInfo.factors);

		SurfaceMaterial_ShadedOpaque_Factors factors{};
		factors.diffuse = diffuseColor;
		factors.SSM = { specular, shininess, metallic };
		factors_buffer->writeToBuffer(&factors);
		factors_buffer->flush();


		materialDescriptorInfo.textureLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorImageInfo diffuseTex_imageInfo = AssetManager::retrieveTexture(diffuseTex)->getImageInfo();
		VkDescriptorImageInfo specularTex_imageInfo = AssetManager::retrieveTexture(specularTex)->getImageInfo();
		VkDescriptorImageInfo shininessTex_imageInfo = AssetManager::retrieveTexture(shininessTex)->getImageInfo();
		VkDescriptorImageInfo metallicTex_imageInfo = AssetManager::retrieveTexture(metallicTex)->getImageInfo();

		VkDescriptorImageInfo normalTex_imageInfo = AssetManager::retrieveTexture(normalTex)->getImageInfo();

		EngineDescriptorWriter(*materialDescriptorInfo.textureLayout, *descriptorPool)
				.writeImage(1, &diffuseTex_imageInfo)
				.writeImage(2, &specularTex_imageInfo)
				.writeImage(3, &shininessTex_imageInfo)
				.writeImage(4, &metallicTex_imageInfo)
				.writeImage(5, &normalTex_imageInfo)
				.build(materialDescriptorInfo.textures);

		materialPipelineConfig = &surfaceShadedOpaque;

		MaterialSystem::createSurfacePipelineLayout(device,
			materialDescriptorInfo.factorLayout->getDescriptorSetLayout(),
			materialDescriptorInfo.textureLayout->getDescriptorSetLayout(),
			&materialPipelineConfig->pipelineLayout
		);
		PipelineConfigInfo pipelineConfigInfo{};
		EnginePipeline::pipelineConfig(pipelineConfigInfo)
			.defaultPipelineConfigInfo()
			.enableVertexDescriptions();
		MaterialSystem::createSurfacePipeline(device, &surfaceShadedOpaque.shaderPipeline, surfaceShadedOpaque.pipelineLayout, pipelineConfigInfo, "assets/shaderdata/materials/surface_shaded_opaque.frag.spv");

		built = true;
	}

	void SurfaceMaterial_ShadedMasked::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
		uPtr<EngineBuffer> factors_buffer =
			make_uPtr<EngineBuffer>(
				device,
				sizeof(SurfaceMaterial_ShadedOpaque_Factors),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

		factors_buffer->map();

		materialDescriptorInfo.factorLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorBufferInfo bufferInfo = factors_buffer->descriptorInfo();
		EngineDescriptorWriter(*materialDescriptorInfo.factorLayout, *SharedPools::staticMaterialPool)
			.writeBuffer(1, &bufferInfo)
			.build(materialDescriptorInfo.factors);

		SurfaceMaterial_ShadedOpaque_Factors factors{};
		factors.diffuse = diffuseColor;
		factors.SSM = { specular, shininess, metallic };
		factors_buffer->writeToBuffer(&factors);
		factors_buffer->flush();


		materialDescriptorInfo.textureLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorImageInfo diffuseTex_imageInfo = AssetManager::retrieveTexture(diffuseTex)->getImageInfo();
		VkDescriptorImageInfo specularTex_imageInfo = AssetManager::retrieveTexture(specularTex)->getImageInfo();
		VkDescriptorImageInfo shininessTex_imageInfo = AssetManager::retrieveTexture(shininessTex)->getImageInfo();
		VkDescriptorImageInfo metallicTex_imageInfo = AssetManager::retrieveTexture(metallicTex)->getImageInfo();

		VkDescriptorImageInfo normalTex_imageInfo = AssetManager::retrieveTexture(normalTex)->getImageInfo();
		VkDescriptorImageInfo maskTex_imageInfo = AssetManager::retrieveTexture(maskTex)->getImageInfo();

		EngineDescriptorWriter(*materialDescriptorInfo.textureLayout, *descriptorPool)
			.writeImage(1, &diffuseTex_imageInfo)
			.writeImage(2, &specularTex_imageInfo)
			.writeImage(3, &shininessTex_imageInfo)
			.writeImage(4, &metallicTex_imageInfo)
			.writeImage(5, &normalTex_imageInfo)
			.writeImage(6, &maskTex_imageInfo)
			.build(materialDescriptorInfo.textures);

		materialPipelineConfig = &surfaceShadedMasked;

		MaterialSystem::createSurfacePipelineLayout(device,
			materialDescriptorInfo.factorLayout->getDescriptorSetLayout(),
			materialDescriptorInfo.textureLayout->getDescriptorSetLayout(),
			&materialPipelineConfig->pipelineLayout
		);
		PipelineConfigInfo pipelineConfigInfo{};
		EnginePipeline::pipelineConfig(pipelineConfigInfo)
			.defaultPipelineConfigInfo()
			.enableVertexDescriptions();
		MaterialSystem::createSurfacePipeline(device, &surfaceShadedMasked.shaderPipeline, surfaceShadedMasked.pipelineLayout, pipelineConfigInfo, "assets/shaderdata/materials/surface_shaded_masked.frag.spv");

		built = true;
	}

	void SurfaceMaterial_ShadedTranslucent::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
		uPtr<EngineBuffer> factors_buffer =
			make_uPtr<EngineBuffer>(
				device,
				sizeof(SurfaceMaterial_ShadedOpaque_Factors),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

		factors_buffer->map();

		materialDescriptorInfo.factorLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorBufferInfo bufferInfo = factors_buffer->descriptorInfo();
		EngineDescriptorWriter(*materialDescriptorInfo.factorLayout, *SharedPools::staticMaterialPool)
			.writeBuffer(1, &bufferInfo)
			.build(materialDescriptorInfo.factors);

		SurfaceMaterial_ShadedOpaque_Factors factors{};
		factors.diffuse = { diffuseColor.x, diffuseColor.y, diffuseColor.z, opacity };
		factors.SSM = { specular, shininess, clarity };
		factors_buffer->writeToBuffer(&factors);
		factors_buffer->flush();


		materialDescriptorInfo.textureLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorImageInfo diffuseTex_imageInfo = AssetManager::retrieveTexture(diffuseTex)->getImageInfo();
		VkDescriptorImageInfo specularTex_imageInfo = AssetManager::retrieveTexture(specularTex)->getImageInfo();
		VkDescriptorImageInfo shininessTex_imageInfo = AssetManager::retrieveTexture(shininessTex)->getImageInfo();
		VkDescriptorImageInfo clarityTex_imageInfo = AssetManager::retrieveTexture(clarityTex)->getImageInfo();

		VkDescriptorImageInfo normalTex_imageInfo = AssetManager::retrieveTexture(normalTex)->getImageInfo();

		EngineDescriptorWriter(*materialDescriptorInfo.textureLayout, *descriptorPool)
			.writeImage(1, &diffuseTex_imageInfo)
			.writeImage(2, &specularTex_imageInfo)
			.writeImage(3, &shininessTex_imageInfo)
			.writeImage(4, &clarityTex_imageInfo)
			.writeImage(5, &normalTex_imageInfo)
			.build(materialDescriptorInfo.textures);

		materialPipelineConfig = &surfaceShadedTranslucent;

		MaterialSystem::createSurfacePipelineLayout(device,
			materialDescriptorInfo.factorLayout->getDescriptorSetLayout(),
			materialDescriptorInfo.textureLayout->getDescriptorSetLayout(),
			&materialPipelineConfig->pipelineLayout
		);
		PipelineConfigInfo pipelineConfigInfo{};
		EnginePipeline::pipelineConfig(pipelineConfigInfo)
			.defaultPipelineConfigInfo()
			.enableAlphaBlending(VK_BLEND_OP_ADD)
			.enableVertexDescriptions();
		MaterialSystem::createSurfacePipeline(device, &surfaceShadedTranslucent.shaderPipeline, surfaceShadedTranslucent.pipelineLayout, pipelineConfigInfo, "assets/shaderdata/materials/surface_shaded_translucent.frag.spv");

		built = true;
	}
#pragma endregion
#pragma region SurfaceUnshaded


#pragma endregion
#pragma endregion


}