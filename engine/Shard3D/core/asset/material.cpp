#include "../../s3dpch.h"
#include "material.h"
#include "assetmgr.h"
#include "../../systems/handlers/material_handler.h"
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

		free();
	}

	void SurfaceMaterial::free() {
		SharedPools::staticMaterialPool->freeDescriptors({ this->materialDescriptors.factors, this->materialDescriptors.textures });
	}

	void SurfaceMaterial::setBlendMode(SurfaceMaterialBlendMode_T mode) {
		bool isNotOpaque = false;
		if (mode & _SurfaceMaterialBlendModeMasked) {
			if (!(classFlags & SurfaceMaterialClassOptions_Masked)) maskedInfo = new _SurfaceMaterial_BlendModeMasked();
			classFlags |= SurfaceMaterialClassOptions_Masked;
			isNotOpaque = true;
		} else {
			classFlags &= ~SurfaceMaterialClassOptions_Masked;
		}
		if (mode & _SurfaceMaterialBlendModeTranslucent) {
			if (!(classFlags & SurfaceMaterialClassOptions_Translucent)) translucentInfo = new _SurfaceMaterial_BlendModeTranslucent();
			classFlags |= SurfaceMaterialClassOptions_Translucent;
			isNotOpaque = true;
		} else {
			classFlags &= ~SurfaceMaterialClassOptions_Translucent;
		}
		if (!isNotOpaque) classFlags |= SurfaceMaterialClassOptions_Opaque;
		else classFlags &= ~SurfaceMaterialClassOptions_Opaque;
	}

	SurfaceMaterialBlendMode_T SurfaceMaterial::getBlendMode() {
		SurfaceMaterialBlendMode_T return_{};
		if (classFlags & SurfaceMaterialClassOptions_Masked)
			return_ |= _SurfaceMaterialBlendModeMasked;
		if (classFlags & SurfaceMaterialClassOptions_Translucent)
			return_ |= _SurfaceMaterialBlendModeTranslucent;
		return return_;
	}

	void SurfaceMaterial::setCullMode(VkCullModeFlags cullmode) {
		if (cullmode == VK_CULL_MODE_NONE) {
			classFlags |= SurfaceMaterialClassOptions_NoCulling;
			classFlags &= ~SurfaceMaterialClassOptions_FrontfaceCulling;
			return;
		}
		if (cullmode == VK_CULL_MODE_FRONT_BIT) {
			classFlags |= SurfaceMaterialClassOptions_FrontfaceCulling;
			classFlags &= ~SurfaceMaterialClassOptions_NoCulling;
			return;
		}
		SHARD3D_ERROR("Invalid cull mode specified");
	}

	VkCullModeFlagBits SurfaceMaterial::getCullMode() {
		if (classFlags & SurfaceMaterialClassOptions_NoCulling)
			return VK_CULL_MODE_NONE;
		if (classFlags & SurfaceMaterialClassOptions_FrontfaceCulling)
			return VK_CULL_MODE_FRONT_BIT;
	}

	void SurfaceMaterial::setPolygonMode(VkPolygonMode polygonmode) {
		SHARD3D_ERROR("Unsupported function");
	}

	VkPolygonMode SurfaceMaterial::getPolygonMode() {
		SHARD3D_ERROR("Unsupported function");
		return VK_POLYGON_MODE_FILL;
	}

	void SurfaceMaterial::bind(VkCommandBuffer commandBuffer) {
		SHARD3D_ASSERT(built && "Material descriptors and pipelines must be built before surface material can be bound!");

		//MaterialHandler::bindMaterialClass(classFlags, commandBuffer);

		VkDescriptorSet sets[] = {
			materialDescriptors.factors,
			materialDescriptors.textures
		};

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			MaterialHandler::getMaterialClass(classFlags)->getPipelineLayout(),
			(classFlags & SurfaceMaterialClassOptions_Translucent) ? 2 : 1,
			2,
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
		if (built) this->free();

		SHARD3D_ASSERT(classFlags & SurfaceMaterialClassOptions_Masked ? (this->maskedInfo != nullptr) : true && "Masked info for surface material not set, while material has Masked as a blend mode set!");
		SHARD3D_ASSERT(classFlags & SurfaceMaterialClassOptions_Translucent ? (this->translucentInfo != nullptr) : true && "Translucent info for surface material not set, while material has Translucent as a blend mode set!");
		SHARD3D_ASSERT(classFlags & SurfaceMaterialClassOptions_FrontfaceCulling || classFlags & SurfaceMaterialClassOptions_NoCulling && "No cull mode has been specified in this material!");

		MaterialHandler::debugPrintFlags(classFlags);

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

			VkDescriptorBufferInfo bufferInfo = factorsBuffer->descriptorInfo();
 				auto& classes = MaterialHandler::getAvailableClasses();
			EngineDescriptorWriter(*MaterialHandler::getMaterialClass(this->classFlags)->getDescriptorLayouts().factorLayout, *SharedPools::staticMaterialPool)
				.writeBuffer(1, &bufferInfo)
				.build(materialDescriptors.factors);

			SurfaceMaterial_ShadedOpaque_Factors factors{};
			factors.diffuse = { this->diffuseColor.x, this->diffuseColor.y, this->diffuseColor.z,
				(classFlags & SurfaceMaterialClassOptions_Translucent) ? this->translucentInfo->opacity : 1.f };
			factors.SSM = { this->specular, this->shininess, this->metallic };
			factorsBuffer->writeToBuffer(&factors);
			factorsBuffer->flush();
		}
			
		{
			VkDescriptorImageInfo diffuseTex_imageInfo = ResourceHandler::retrieveTexture(this->diffuseTex)->getImageInfo();
			VkDescriptorImageInfo specularTex_imageInfo = ResourceHandler::retrieveTexture(this->specularTex)->getImageInfo();
			VkDescriptorImageInfo shininessTex_imageInfo = ResourceHandler::retrieveTexture(this->shininessTex)->getImageInfo();
			VkDescriptorImageInfo metallicTex_imageInfo = ResourceHandler::retrieveTexture(this->metallicTex)->getImageInfo();
			VkDescriptorImageInfo normalTex_imageInfo = ResourceHandler::retrieveTexture(this->normalTex)->getImageInfo();

			EngineDescriptorWriter textureLayout_writer = EngineDescriptorWriter(*MaterialHandler::getMaterialClass(this->classFlags)->getDescriptorLayouts().textureLayout, *descriptorPool);
			textureLayout_writer.writeImage(1, &diffuseTex_imageInfo);
			textureLayout_writer.writeImage(2, &specularTex_imageInfo);
			textureLayout_writer.writeImage(3, &shininessTex_imageInfo);
			textureLayout_writer.writeImage(4, &metallicTex_imageInfo);
			textureLayout_writer.writeImage(5, &normalTex_imageInfo);

			if (classFlags & SurfaceMaterialClassOptions_Masked) {
				VkDescriptorImageInfo maskTex_imageInfo = ResourceHandler::retrieveTexture(this->maskedInfo->maskTex)->getImageInfo();
				textureLayout_writer.writeImage(6, &maskTex_imageInfo);
			}
			if (classFlags & SurfaceMaterialClassOptions_Translucent) {
				VkDescriptorImageInfo opacityTex_imageInfo = ResourceHandler::retrieveTexture(this->translucentInfo->opacityTex)->getImageInfo();
				textureLayout_writer.writeImage(7, &opacityTex_imageInfo);
			}

			textureLayout_writer.build(materialDescriptors.textures);
		}

		built = true;
	}

	void SurfaceMaterial_Shaded::serialize(YAML::Emitter* out) {
		SHARD3D_ASSERT(classFlags & SurfaceMaterialClassOptions_Masked ? (this->maskedInfo != nullptr) : true && "Masked info for surface material not set, while material has Masked as a blend mode set!");
		SHARD3D_ASSERT(classFlags & SurfaceMaterialClassOptions_Translucent ? (this->translucentInfo != nullptr) : true && "Translucent info for surface material not set, while material has Translucent as a blend mode set!");

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

		if (classFlags & SurfaceMaterialClassOptions_Masked)
			*out << YAML::Key << "maskTex" << YAML::Value << this->maskedInfo->maskTex.getFile();
		if (classFlags & SurfaceMaterialClassOptions_Translucent) {
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

		if (classFlags & SurfaceMaterialClassOptions_Masked)
			this->maskedInfo->maskTex = (*data)["Data"]["maskTex"].as<std::string>();

		if (classFlags & SurfaceMaterialClassOptions_Translucent) {
			this->translucentInfo->opacity = (*data)["Data"]["opacity"].as<float>();
			this->translucentInfo->opacityTex = (*data)["Data"]["opacityTex"].as<std::string>();
		}
	}

	void SurfaceMaterial_Shaded::loadAllTextures() {
		SHARD3D_ASSERT(classFlags & SurfaceMaterialClassOptions_Masked ? (this->maskedInfo != nullptr) : true && "Masked info for surface material not set, while material has Masked as a blend mode set!");
		SHARD3D_ASSERT(classFlags & SurfaceMaterialClassOptions_Translucent ? (this->translucentInfo != nullptr) : true && "Translucent info for surface material not set, while material has Translucent as a blend mode set!");

		ResourceHandler::loadTexture(this->normalTex);
		ResourceHandler::loadTexture(this->diffuseTex);
		ResourceHandler::loadTexture(this->specularTex);
		ResourceHandler::loadTexture(this->shininessTex);
		ResourceHandler::loadTexture(this->metallicTex);

		if (classFlags & SurfaceMaterialClassOptions_Masked)
			ResourceHandler::loadTexture(this->maskedInfo->maskTex);
		if (classFlags & SurfaceMaterialClassOptions_Translucent)
			ResourceHandler::loadTexture(this->translucentInfo->opacityTex);
	}

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
		
		for (auto& param : myParams) {
			param.free();
		}
		free();
	}	
	
	void PostProcessingMaterial::free() {
		MaterialHandler::destroyPipelineLayout(materialPipelineConfig->shaderPipelineLayout);
		//if (SharedPools::staticMaterialPool)
			SharedPools::staticMaterialPool->freeDescriptors({ this->materialDescriptorInfo.params });
	}

	void PostProcessingMaterial::createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) {
		if (built) this->free();
		
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
		MaterialHandler::createPPOPipelineLayout(
			&materialPipelineConfig->shaderPipelineLayout,
			materialDescriptorInfo.paramsLayout->getDescriptorSetLayout()
		);
		
		if (strUtils::hasEnding(this->shaderPath, ".spv"))
			MaterialHandler::createPPOPipeline(
				&materialPipelineConfig->shaderPipeline,
				materialPipelineConfig->shaderPipelineLayout,
				this->shaderPath);
		else {
			MaterialHandler::createPPOPipeline(
				&materialPipelineConfig->shaderPipeline,
				materialPipelineConfig->shaderPipelineLayout,
				ShaderSystem::compileOnTheFly(this->shaderPath, ShaderType::Compute));
		}
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
		vkCmdDispatch(commandBuffer, GraphicsSettings::getRuntimeInfo().PostProcessingInvocationIDCounts.x, GraphicsSettings::getRuntimeInfo().PostProcessingInvocationIDCounts.y, 1);
		SHARD3D_STAT_RECORD_END({ "Post processing", "Dispatch " + this->master->materialTag });

	}
	RandomShaderParam& PostProcessingMaterialInstance::getParameter(uint32_t index) {
		return myParamsLocal[index];
	}
#pragma endregion
}