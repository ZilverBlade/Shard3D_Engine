#include "../../s3dpch.h"
#include "material.h"
#include "assetmgr.h"
#include "../../systems/buffers/material_system.h"

namespace Shard3D {

#pragma region Surface

	SurfaceMaterial::SurfaceMaterial() { }

	SurfaceMaterial::~SurfaceMaterial() {
		if (!built) { 
			SHARD3D_WARN("Attempted to destroy pipeline while material has never been built!");
			return;
		}
		
		if (maskedInfo)
			delete maskedInfo;
		if (translucentInfo)
			delete translucentInfo;

		MaterialSystem::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);
	}

	void SurfaceMaterial::setBlendMode(SurfaceMaterialBlendMode_T mode) {
		if (mode & SurfaceMaterialBlendModeMasked && !(blendMode & mode))
			maskedInfo = new SurfaceMaterial_BlendModeMasked();
		if (mode & SurfaceMaterialBlendModeTranslucent && !(blendMode & mode))
			translucentInfo = new SurfaceMaterial_BlendModeTranslucent();
		blendMode = mode;
	}

	void SurfaceMaterial::bind(VkCommandBuffer commandBuffer, VkDescriptorSet globalSet) {
		SHARD3D_ASSERT(built && "Material descriptors and pipelines must be built before surface material can be bound!");

		// TODO: change this to cache certain pipelines to speed up binding
		// Or, change this that the model.cpp file submits the primitives to some sort of unordered map to render, categorised by the pipelines,
		// however this wont work when custom material shaders files can be created.
		// Instead make Uber shaders where all of the materials are stored in 1 shader

		materialPipelineConfig->shaderPipeline->bind(commandBuffer);

		VkDescriptorSet sets[] = {
			globalSet,
			materialDescriptorInfo.factors,
			materialDescriptorInfo.textures
		};

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			materialPipelineConfig->shaderPipelineLayout,
			0,
			3,
			sets,
			0,
			nullptr
		);
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

	void SurfaceMaterial_Shaded::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
		if (built) MaterialSystem::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);

		if (blendMode & SurfaceMaterialBlendModeMasked)
			SHARD3D_ASSERT(this->maskedInfo && "Masked info for surface material not set, while material has Masked as a blend mode set!");
		if (blendMode & SurfaceMaterialBlendModeTranslucent)
			SHARD3D_ASSERT(this->translucentInfo && "Translucent info for surface material not set, while material has Translucent as a blend mode set!");

		{
			factorsBuffer =
				make_uPtr<EngineBuffer>(
					device,
					sizeof(SurfaceMaterial_ShadedOpaque_Factors),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					);

			factorsBuffer->map();

			materialDescriptorInfo.factorLayout = EngineDescriptorSetLayout::Builder(device)
				.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();

			VkDescriptorBufferInfo bufferInfo = factorsBuffer->descriptorInfo();
			EngineDescriptorWriter(*materialDescriptorInfo.factorLayout, *descriptorPool)
				.writeBuffer(1, &bufferInfo)
				.build(materialDescriptorInfo.factors);

			SurfaceMaterial_ShadedOpaque_Factors factors{};
			factors.diffuse = { this->diffuseColor.x, this->diffuseColor.y, this->diffuseColor.z,
				(blendMode == SurfaceMaterialBlendModeTranslucent) ? this->translucentInfo->opacity : 1.f };
			factors.SSM = { this->specular, this->shininess, this->metallic };
			factorsBuffer->writeToBuffer(&factors);
			factorsBuffer->flush();
		}

		{
			EngineDescriptorSetLayout::Builder textureLayout_builder = EngineDescriptorSetLayout::Builder(device);
			textureLayout_builder.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			if (blendMode == SurfaceMaterialBlendModeMasked)
				textureLayout_builder.addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			if (blendMode == SurfaceMaterialBlendModeTranslucent)
				textureLayout_builder.addBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

			materialDescriptorInfo.textureLayout = textureLayout_builder.build();
		}

		{
			VkDescriptorImageInfo diffuseTex_imageInfo = ResourceHandler::retrieveTexture(this->diffuseTex)->getImageInfo();
			VkDescriptorImageInfo specularTex_imageInfo = ResourceHandler::retrieveTexture(this->specularTex)->getImageInfo();
			VkDescriptorImageInfo shininessTex_imageInfo = ResourceHandler::retrieveTexture(this->shininessTex)->getImageInfo();
			VkDescriptorImageInfo metallicTex_imageInfo = ResourceHandler::retrieveTexture(this->metallicTex)->getImageInfo();
			VkDescriptorImageInfo normalTex_imageInfo = ResourceHandler::retrieveTexture(this->normalTex)->getImageInfo();

			EngineDescriptorWriter textureLayout_writer = EngineDescriptorWriter(*materialDescriptorInfo.textureLayout, *descriptorPool);
			textureLayout_writer.writeImage(1, &diffuseTex_imageInfo);
			textureLayout_writer.writeImage(2, &specularTex_imageInfo);
			textureLayout_writer.writeImage(3, &shininessTex_imageInfo);
			textureLayout_writer.writeImage(4, &metallicTex_imageInfo);
			textureLayout_writer.writeImage(5, &normalTex_imageInfo);

			if (blendMode & SurfaceMaterialBlendModeMasked) {
				VkDescriptorImageInfo maskTex_imageInfo = ResourceHandler::retrieveTexture(this->maskedInfo->maskTex)->getImageInfo();
				textureLayout_writer.writeImage(6, &maskTex_imageInfo);
			}
			if (blendMode & SurfaceMaterialBlendModeTranslucent) {
				VkDescriptorImageInfo opacityTex_imageInfo = ResourceHandler::retrieveTexture(this->translucentInfo->opacityTex)->getImageInfo();
				textureLayout_writer.writeImage(7, &opacityTex_imageInfo);
			}

			textureLayout_writer.build(materialDescriptorInfo.textures);
		}

		materialPipelineConfig = make_uPtr<MaterialGraphicsPipelineConfigInfo>();
		MaterialSystem::createSurfacePipelineLayout(
			materialDescriptorInfo.factorLayout->getDescriptorSetLayout(),
			materialDescriptorInfo.textureLayout->getDescriptorSetLayout(),
			&materialPipelineConfig->shaderPipelineLayout
		);
		GraphicsPipelineConfigInfo pipelineConfigInfo{};
		GraphicsPipeline::pipelineConfig(pipelineConfigInfo)
			.defaultGraphicsPipelineConfigInfo()
			.setCullMode(drawData.culling)
			.enableVertexDescriptions();

		if (this->blendMode == SurfaceMaterialBlendModeTranslucent)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).enableAlphaBlending(VK_BLEND_OP_ADD);

		std::string shader = "assets/shaderdata/materials/surface_shaded_opaque.frag.spv";
		
		if (blendMode & SurfaceMaterialBlendModeMasked) 
			shader = "assets/shaderdata/materials/surface_shaded_masked.frag.spv";		
		if (blendMode & SurfaceMaterialBlendModeTranslucent) 
			shader = "assets/shaderdata/materials/surface_shaded_translucent.frag.spv";	

		MaterialSystem::createSurfacePipeline(
			&materialPipelineConfig->shaderPipeline,
			materialPipelineConfig->shaderPipelineLayout, 
			pipelineConfigInfo, 
			shader);

		built = true;
	}

	void SurfaceMaterial_Shaded::serialize(YAML::Emitter* out) {
		if (blendMode & SurfaceMaterialBlendModeMasked)
			SHARD3D_ASSERT(this->maskedInfo && "Masked info for surface material not set, while material has Masked as a blend mode set!");
		if (blendMode & SurfaceMaterialBlendModeTranslucent)
			SHARD3D_ASSERT(this->translucentInfo && "Translucent info for surface material not set, while material has Translucent as a blend mode set!");
		
		*out << YAML::BeginMap;
		*out << YAML::Key << "diffuseColor" << YAML::Value << this->diffuseColor;
		*out << YAML::Key << "diffuseTex" << YAML::Value << this->diffuseTex.getFile();
		*out << YAML::Key << "specularFactor" << YAML::Value << this->specular;
		*out << YAML::Key << "specularTex" << YAML::Value << this->specularTex.getFile();
		*out << YAML::Key << "shininessFactor" << YAML::Value << this->shininess;
		*out << YAML::Key << "shininessTex" << YAML::Value << this->shininessTex.getFile();
		*out << YAML::Key << "metallicFactor" << YAML::Value << this->metallic;
		*out << YAML::Key << "metallicTex" << YAML::Value << this->metallicTex.getFile();	
		*out << YAML::Key << "normalTex" << YAML::Value << this->normalTex.getFile();

		if (blendMode == SurfaceMaterialBlendModeMasked)
			*out << YAML::Key << "maskTex" << YAML::Value << this->maskedInfo->maskTex.getFile();
		if (blendMode == SurfaceMaterialBlendModeTranslucent) {
			*out << YAML::Key << "opacityTex" << YAML::Value << this->translucentInfo->opacityTex.getFile();
			*out << YAML::Key << "opacity" << YAML::Value << this->translucentInfo->opacity;
		}
		*out << YAML::EndMap;
	}

	void SurfaceMaterial_Shaded::deserialize(YAML::Node* data) {
		this->diffuseColor = (*data)["Data"]["diffuseColor"].as<glm::vec3>();
		this->specular = (*data)["Data"]["specularFactor"].as<float>();
		this->shininess = (*data)["Data"]["shininessFactor"].as<float>();
		this->metallic = (*data)["Data"]["metallicFactor"].as<float>();
		
		this->normalTex = (*data)["Data"]["normalTex"].as<std::string>();
		this->diffuseTex = (*data)["Data"]["diffuseTex"].as<std::string>();
		this->specularTex = (*data)["Data"]["specularTex"].as<std::string>();
		this->shininessTex = (*data)["Data"]["shininessTex"].as<std::string>();
		this->metallicTex = (*data)["Data"]["metallicTex"].as<std::string>();

		if (blendMode & SurfaceMaterialBlendModeMasked)
			this->maskedInfo->maskTex = (*data)["Data"]["maskTex"].as<std::string>();

		if (blendMode & SurfaceMaterialBlendModeTranslucent) {
			this->translucentInfo->opacity = (*data)["Data"]["opacity"].as<float>();
			this->translucentInfo->opacityTex = (*data)["Data"]["opacityTex"].as<std::string>();
		}
	}

	void SurfaceMaterial_Shaded::loadAllTextures() {
		if (blendMode & SurfaceMaterialBlendModeMasked)
			SHARD3D_ASSERT(this->maskedInfo && "Masked info for surface material not set, while material has Masked as a blend mode set!");
		if (blendMode & SurfaceMaterialBlendModeTranslucent)
			SHARD3D_ASSERT(this->translucentInfo && "Translucent info for surface material not set, while material has Translucent as a blend mode set!");
		
		ResourceHandler::loadTexture(this->normalTex);
		ResourceHandler::loadTexture(this->diffuseTex);
		ResourceHandler::loadTexture(this->specularTex);
		ResourceHandler::loadTexture(this->shininessTex);
		ResourceHandler::loadTexture(this->metallicTex);

		if (blendMode & SurfaceMaterialBlendModeMasked)
			ResourceHandler::loadTexture(this->maskedInfo->maskTex);
		if (blendMode & SurfaceMaterialBlendModeTranslucent)
			ResourceHandler::loadTexture(this->translucentInfo->opacityTex);
	}
	// experimental
	/*
		void SurfaceMaterial_ShadedMasked::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
			// pseudo code

			std::string foo{};
			std::string foo2{};

			foo = "shaders/__experimental/blueprints/surface_shaded.frag";

			if (type == shadedopaque)
			foo2 = "shaders/__experimental/blueprints/surface_opaque.frag";

			built = true;
		}
	*/


#pragma endregion
#pragma region SurfaceUnshaded


#pragma endregion
#pragma endregion

}