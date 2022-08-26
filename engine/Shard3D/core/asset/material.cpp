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
		MaterialSystem::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);
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
	struct SurfaceMaterial_Unshaded_Factors {
		glm::vec4 emissive{ 1.f, 1.f, 1.f, 1.f };
	};

	void SurfaceMaterial_ShadedOpaque::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
		if (built) MaterialSystem::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);

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
		factors.diffuse = { this->diffuseColor.x, this->diffuseColor.y, this->diffuseColor.z, 1.f };
		factors.SSM = { this->specular, this->shininess, this->metallic };
		factorsBuffer->writeToBuffer(&factors);
		factorsBuffer->flush();


		materialDescriptorInfo.textureLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorImageInfo diffuseTex_imageInfo = ResourceHandler::retrieveTexture(this->diffuseTex)->getImageInfo();
		VkDescriptorImageInfo specularTex_imageInfo = ResourceHandler::retrieveTexture(this->specularTex)->getImageInfo();
		VkDescriptorImageInfo shininessTex_imageInfo = ResourceHandler::retrieveTexture(this->shininessTex)->getImageInfo();
		VkDescriptorImageInfo metallicTex_imageInfo = ResourceHandler::retrieveTexture(this->metallicTex)->getImageInfo();

		VkDescriptorImageInfo normalTex_imageInfo = ResourceHandler::retrieveTexture(this->normalTex)->getImageInfo();

		EngineDescriptorWriter(*materialDescriptorInfo.textureLayout, *descriptorPool)
				.writeImage(1, &diffuseTex_imageInfo)
				.writeImage(2, &specularTex_imageInfo)
				.writeImage(3, &shininessTex_imageInfo)
				.writeImage(4, &metallicTex_imageInfo)
				.writeImage(5, &normalTex_imageInfo)
				.build(materialDescriptorInfo.textures);

		materialPipelineConfig = make_uPtr<MaterialPipelineConfigInfo>();
		MaterialSystem::createSurfacePipelineLayout(
			materialDescriptorInfo.factorLayout->getDescriptorSetLayout(),
			materialDescriptorInfo.textureLayout->getDescriptorSetLayout(),
			&materialPipelineConfig->shaderPipelineLayout
		);
		PipelineConfigInfo pipelineConfigInfo{};
		EnginePipeline::pipelineConfig(pipelineConfigInfo)
			.defaultPipelineConfigInfo()
			.enableVertexDescriptions();
		MaterialSystem::createSurfacePipeline(
			&materialPipelineConfig->shaderPipeline,
			materialPipelineConfig->shaderPipelineLayout, 
			pipelineConfigInfo, 
			"assets/shaderdata/materials/surface_shaded_opaque.frag.spv");

		built = true;
	}

	void SurfaceMaterial_ShadedOpaque::serialize(YAML::Emitter* out) {
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
		*out << YAML::EndMap;
	}

	void SurfaceMaterial_ShadedOpaque::deserialize(YAML::Node* data) {
		this->diffuseColor = (*data)["Data"]["diffuseColor"].as<glm::vec3>();
		this->specular = (*data)["Data"]["specularFactor"].as<float>();
		this->shininess = (*data)["Data"]["shininessFactor"].as<float>();
		this->metallic = (*data)["Data"]["metallicFactor"].as<float>();
		
		this->normalTex = (*data)["Data"]["normalTex"].as<std::string>();
		this->diffuseTex = (*data)["Data"]["diffuseTex"].as<std::string>();
		this->specularTex = (*data)["Data"]["specularTex"].as<std::string>();
		this->shininessTex = (*data)["Data"]["shininessTex"].as<std::string>();
		this->metallicTex = (*data)["Data"]["metallicTex"].as<std::string>();
	}

	void SurfaceMaterial_ShadedOpaque::loadAllTextures() {
		ResourceHandler::loadTexture(this->normalTex);
		ResourceHandler::loadTexture(this->diffuseTex);
		ResourceHandler::loadTexture(this->specularTex);
		ResourceHandler::loadTexture(this->shininessTex);
		ResourceHandler::loadTexture(this->metallicTex);
	}


#pragma endregion
#pragma region SurfaceUnshaded


#pragma endregion
#pragma endregion

}