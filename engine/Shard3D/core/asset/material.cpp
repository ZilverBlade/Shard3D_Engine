#include "../../s3dpch.h"
#include "material.h"
#include "assetmgr.h"
#include "../../systems/buffers/material_system.h"
#include "../misc/graphics_settings.h"
#include <any>
#include "../../systems/computational/shader_system.h"

namespace Shard3D {


	void RandomShaderTextureSampler2D::reload() {
		ResourceHandler::loadTexture(mySampler);
	}

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

		SHARD3D_LOG("Destroying Material {0}", this->materialTag);
		MaterialSystem::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);
	}

	void SurfaceMaterial::setBlendMode(SurfaceMaterialBlendMode_T mode) {
		if (mode & SurfaceMaterialBlendModeMasked && !(blendMode & mode))
			maskedInfo = new _SurfaceMaterial_BlendModeMasked();
		if (mode & SurfaceMaterialBlendModeTranslucent && !(blendMode & mode))
			translucentInfo = new _SurfaceMaterial_BlendModeTranslucent();
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

		materialPipelineConfig = make_uPtr<_MaterialGraphicsPipelineConfigInfo>();
		MaterialSystem::createSurfacePipelineLayout(
			&materialPipelineConfig->shaderPipelineLayout,
			materialDescriptorInfo.factorLayout->getDescriptorSetLayout(),
			materialDescriptorInfo.textureLayout->getDescriptorSetLayout()
		);
		GraphicsPipelineConfigInfo pipelineConfigInfo{};
		GraphicsPipeline::pipelineConfig(pipelineConfigInfo)
			.defaultGraphicsPipelineConfigInfo()
			.setCullMode(drawData.culling)
			.enableVertexDescriptions();

		
		if (this->blendMode == SurfaceMaterialBlendModeTranslucent)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).enableAlphaBlending(VK_BLEND_OP_ADD);
	
		pipelineConfigInfo.colorBlendInfo.attachmentCount = 4;
		VkPipelineColorBlendAttachmentState attachments[4]{ pipelineConfigInfo.colorBlendAttachment, pipelineConfigInfo.colorBlendAttachment,pipelineConfigInfo.colorBlendAttachment ,pipelineConfigInfo.colorBlendAttachment };
		pipelineConfigInfo.colorBlendInfo.pAttachments = attachments;


		MaterialSystem::createSurfacePipeline(
			&materialPipelineConfig->shaderPipeline,
			materialPipelineConfig->shaderPipelineLayout, 
			pipelineConfigInfo, 
			this);
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

#pragma region PostProcessing
	PostProcessingMaterial::~PostProcessingMaterial() {
		if (!built) {
			SHARD3D_WARN("Attempted to destroy pipeline while material has never been built!");
			return;
		}
		SHARD3D_LOG("Destroying Material {0}", this->materialTag);
		MaterialSystem::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);

		for (auto& param : myParams) {
			param.free();
		}
	}
	void PostProcessingMaterial::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
		if (built) MaterialSystem::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);
		
		if (!IOUtils::doesFileExist(shaderPath)) {
			SHARD3D_WARN("No shader found!");
			return;
		}

		uint64_t sizeOfMyParams{};
		std::vector<void*> myDataPure{};

		for (auto& param : myParams) {
			myDataPure.push_back(param.get());
			sizeOfMyParams += param.getGPUSize();
		}
		
		myParamsBuffer =
			make_uPtr<EngineBuffer>(
				device,
				(sizeOfMyParams > 0) ? sizeOfMyParams : 4,
				1,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

		myParamsBuffer->map();

		materialDescriptorInfo.paramsLayout = EngineDescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.build();

		VkDescriptorBufferInfo bufferInfo = myParamsBuffer->descriptorInfo();
		EngineDescriptorWriter(*materialDescriptorInfo.paramsLayout, *descriptorPool)
			.writeBuffer(0, &bufferInfo)
			.build(materialDescriptorInfo.params);
		int fake = 42352;
		myParamsBuffer->writeToBuffer((sizeOfMyParams > 0) ? myDataPure.data() : (void*)&fake);
		myParamsBuffer->flush();
		
		materialPipelineConfig = make_uPtr<_MaterialComputePipelineConfigInfo>();
		MaterialSystem::createPPOPipelineLayout(
			&materialPipelineConfig->shaderPipelineLayout,
			materialDescriptorInfo.paramsLayout->getDescriptorSetLayout()
		);
		
		if (strUtils::hasEnding(this->shaderPath, ".spv"))
			MaterialSystem::createPPOPipeline(
				&materialPipelineConfig->shaderPipeline,
				materialPipelineConfig->shaderPipelineLayout,
				this->shaderPath);
		else
			MaterialSystem::createPPOPipeline(
				&materialPipelineConfig->shaderPipeline,
				materialPipelineConfig->shaderPipelineLayout,
				ShaderSystem::compileOnTheFly(this->shaderPath, ShaderType::Compute));

		built = true;
	}

	
	void PostProcessingMaterial::rmvParameter(uint32_t index) {
		SHARD3D_WARN("Removing param {0}", index);
		std::vector<RandomShaderParam>::iterator q = myParams.begin();
		for (int i = 0; i < myParams.size(); i++) {
			if (i == index) break;
			q++;
		}
		myParams.erase(q);
	}
	RandomShaderParam& PostProcessingMaterial::getParameter(uint32_t index) {
		return myParams[index];
	}


	std::string& PostProcessingMaterial::getParameterName(uint32_t index) {
		return myParamNames[index];
	}

	PostProcessingMaterialInstance::PostProcessingMaterialInstance(PostProcessingMaterial* masterMaterial, AssetID masterMaterialAsset) : master(masterMaterial), masterAsset(masterMaterialAsset) {
		myParamsLocal.reserve(master->myParams.size());
		for (auto& param : master->myParams) {
			myParamsLocal.push_back(param.copy());
		}
	}

	PostProcessingMaterialInstance::~PostProcessingMaterialInstance() {}

	void PostProcessingMaterialInstance::updateBuffers() {
		std::vector<uint8_t> myDataPure{};
		myDataPure.reserve(this->master->myParamsBuffer->getBufferSize());
		for (auto& param : myParamsLocal) {
			for (int i = 0; i < param.getGPUSize(); i++)
				myDataPure.push_back(reinterpret_cast<uint8_t*>(param.get())[i]);
		}
		this->master->myParamsBuffer->writeToBuffer(myDataPure.data());
		this->master->myParamsBuffer->flush();
	}

	void PostProcessingMaterialInstance::dispatch(VkCommandBuffer commandBuffer, VkDescriptorSet sceneRenderedSet) {
		SHARD3D_STAT_RECORD();
		this->master->materialPipelineConfig->shaderPipeline->bindCompute(commandBuffer);
		SHARD3D_STAT_RECORD_END({ "Post processing", "Pipeline binding " + this->master->materialTag });

		SHARD3D_STAT_RECORD();
		updateBuffers();
		SHARD3D_STAT_RECORD_END({ "Post processing", "SSBO update" + this->master->materialTag });

		SHARD3D_STAT_RECORD();
		VkDescriptorSet sets[] = {
			sceneRenderedSet,
			this->master->materialDescriptorInfo.params
		};

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_COMPUTE,
			this->master->materialPipelineConfig->shaderPipelineLayout,
			0,
			2,
			sets,
			0,
			nullptr
		);
		SHARD3D_STAT_RECORD_END({"Post processing", "Descriptor set binding " + this->master->materialTag});

		SHARD3D_STAT_RECORD();
		// vkCmdDispatch(commandBuffer, GraphicsSettings::get().WindowWidth / 16, GraphicsSettings::get().WindowHeight / 16, 1);
		vkCmdDispatch(commandBuffer, 1920 / 16, 1080 / 16 + 1, 1);
		SHARD3D_STAT_RECORD_END({ "Post processing", "Dispatch " + this->master->materialTag });

	}
	RandomShaderParam& PostProcessingMaterialInstance::getParameter(uint32_t index) {
		return myParamsLocal[index];
	}
#pragma endregion
}