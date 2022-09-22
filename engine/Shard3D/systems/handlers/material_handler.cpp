#include "../../s3dpch.h" 

#include "material_handler.h"
#include "render_handler.h"
#include "../../core/asset/assetmgr.h"
#include <fstream>
#include "../computational/shader_system.h"
#include "../rendering/deferred_render_system.h"

#define MAX_SUPPORTED_SAMPLER2D_COUNT_VIRTUAL_UBER 16
namespace Shard3D {
	void MaterialHandler::setAllAvailableMaterialShaderPermutations(std::vector<SurfaceMaterialClassOptionsFlags>&& shaders) {
		RenderHandler::materialRendering.reserve(shaders.size() * 2);
		for (uint32_t permutation_ : shaders) {
			for (int i = 0; i < 2 ; i++) {
				uint32_t permutation = permutation_ | (i ? SurfaceMaterialClassOptions_FrontfaceCulling : SurfaceMaterialClassOptions_NoCulling);
				RenderHandler::materialRendering[permutation] = hashMap<UUID, SurfaceMaterialRenderInfo>();

				SurfaceMaterialClass* xClass;
				
				if (permutation_ & SurfaceMaterialClassOptions_Translucent) {
					xClass = new SurfaceMaterialClassForward(*mDevice, permutation);
					materialClassesForward[permutation] = reinterpret_cast<SurfaceMaterialClassForward*>(xClass);
					materialClassesOptionsForward.push_back(permutation);
				}
				else {
					xClass = new SurfaceMaterialClassDeferred(*mDevice, permutation);
					materialClassesDeferred[permutation] = reinterpret_cast<SurfaceMaterialClassDeferred*>(xClass);
					materialClassesOptionsDeferred.push_back(permutation);
				}
				materialClassesOptionsGlobal.push_back(permutation);
				materialClassesGlobal[permutation] = xClass;
			}
		}
		materialClassDeferredLighting = new SurfaceMaterialClassDeferredLighting(*mDevice);
	}
	void MaterialHandler::destroy() {
		for (auto& [flags, mas_materials] : RenderHandler::materialRendering) {
			mas_materials.clear();
		}
		RenderHandler::materialRendering.clear();
		for (auto& [_, item] : materialClassesGlobal) {
			delete item;
		}
		materialClassesForward.clear();
		materialClassesDeferred.clear();
		delete materialClassDeferredLighting;
	}
	void MaterialHandler::recompileSurface() {
		compiledShaders.clear();
		materialClassesGlobal.clear();
		materialClassesDeferred.clear();
		materialClassesForward.clear();
		for (auto& [key, class_] : MaterialHandler::materialClassesGlobal) {
			delete class_;
		}
		for (uint32_t permutation : materialClassesOptionsGlobal) {
			RenderHandler::materialRendering[permutation] = hashMap<UUID, SurfaceMaterialRenderInfo>();

			SurfaceMaterialClass* xClass;

			if (permutation & SurfaceMaterialClassOptions_Translucent) {
				xClass = new SurfaceMaterialClassForward(*mDevice, permutation);
				materialClassesForward[permutation] = reinterpret_cast<SurfaceMaterialClassForward*>(xClass);
			}
			else {
				xClass = new SurfaceMaterialClassDeferred(*mDevice, permutation);
				materialClassesDeferred[permutation] = reinterpret_cast<SurfaceMaterialClassDeferred*>(xClass);
			}
			materialClassesGlobal[permutation] = xClass;
		}
		delete materialClassDeferredLighting;
		materialClassDeferredLighting = new SurfaceMaterialClassDeferredLighting(*mDevice);

		for (auto& [key, material] : ResourceHandler::getSurfaceMaterialAssets()) {
			ResourceHandler::rebuildSurfaceMaterial(material);
		}
	}
	void MaterialHandler::recompilePPO() {
		for (auto& [key, material] : ResourceHandler::getPPOMaterialAssets()) {
			ResourceHandler::rebuildPPOMaterial(material);
		}
	}

	void MaterialHandler::createSurfacePipelineLayout(VkPipelineLayout* pipelineLayout, std::vector<VkDescriptorSetLayout> layouts, VkPushConstantRange* pushConstantRange) {
		SHARD3D_ASSERT(mRenderPass != VK_NULL_HANDLE && mGlobalSetLayout != VK_NULL_HANDLE && mDevice != VK_NULL_HANDLE && "Material system context not set!");

		SHARD3D_ASSERT(pipelineLayout != nullptr && "No pipeline layout provided!");
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};

		descriptorSetLayouts.reserve(layouts.size() + 1);
		descriptorSetLayouts.push_back(mGlobalSetLayout);
		for (int i = 0; i < layouts.size(); i++) descriptorSetLayouts.push_back(layouts[i]);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = pushConstantRange? 1 : 0;
		pipelineLayoutInfo.pPushConstantRanges = pushConstantRange;
		if (vkCreatePipelineLayout(mDevice->device(), &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}
	void MaterialHandler::createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, 
		GraphicsPipelineConfigInfo& pipelineConfig, const std::string& fragment_shader, const std::string& vertex_shader) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		pipelineConfig.renderPass = mRenderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		*pipeline = make_uPtr<GraphicsPipeline>(
			*mDevice,
			vertex_shader,
			fragment_shader,
			pipelineConfig
		);
	}


	void MaterialHandler::createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, GraphicsPipelineConfigInfo& pipelineConfig, SurfaceMaterialClass* self, uint8_t subpassIndex) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		pipelineConfig.renderPass = mRenderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		std::stringstream stream;
		std::stringstream cacheDir;
		std::string finalFile;
		cacheDir << "assets/shaderdata/materials/surface";
		stream << "#version 450\n";
		switch (subpassIndex) {
		case(0):
			cacheDir << "_deferred";
			break;
		case(2):
			cacheDir << "_forward";
			break;
		}
		if (self->getClass() & SurfaceMaterialClassOptions_Shaded) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED\n";
			cacheDir << "_shaded";
		} if (self->getClass() & SurfaceMaterialClassOptions_Unshaded) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_EMISSIVE\n";
			cacheDir << "_emissive";
		} if (self->getClass() & SurfaceMaterialClassOptions_Masked) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED\n";
			cacheDir << "_masked";
		} if (self->getClass() & SurfaceMaterialClassOptions_Translucent) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_TRANSLUCENT\n";
			cacheDir << "_translucent";
		} if (self->getClass() & SurfaceMaterialClassOptions_Opaque)
			cacheDir << "_opaque";

		cacheDir << ".spv";

		if (finalFile.empty())
			finalFile = cacheDir.str();

		if (std::find(compiledShaders.begin(), compiledShaders.end(), finalFile) != compiledShaders.end()) {
			SHARD3D_LOG("Reading cached shader '{0}'", finalFile);
			goto pipelinecreate;
		}

		{		
			stream << "//";
			std::string file;
			// comment out the version 
			switch (subpassIndex) {
			case(0):
				file = "assets/_engine/shaderdata/surface_material/surface_material_deferred.frag";
				stream << IOUtils::readText(file);
				break;
			case(2):
				file = "assets/_engine/shaderdata/surface_material/surface_material_forward.frag";
				stream << IOUtils::readText(file);
				break;
			}
			
			std::string finalString = stream.rdbuf()->str();
			finalString = finalString.substr(0, finalString.length() - 2);

			std::vector<char> myBytes = ShaderSystem::compileOnTheFlyDirect(finalString, file.c_str(), ShaderType::Pixel);
			if (!myBytes.empty()) {
				IOUtils::writeStackBinary(myBytes.data(), myBytes.size(), finalFile);
			}
			else {
				SHARD3D_ERROR("Shader compiling for material class '{0}' failed!! Reverting to previous best shader", finalFile);
			}
			compiledShaders.push_back(cacheDir.str());
		}
	pipelinecreate:
		*pipeline = make_uPtr<GraphicsPipeline>(
			*mDevice,
			"assets/shaderdata/mesh_shader.vert.spv",
			finalFile,
			pipelineConfig
			);

	}


	void MaterialHandler::createPPOPipelineLayout(VkPipelineLayout* pipelineLayout, VkDescriptorSetLayout dataLayout) {
		SHARD3D_ASSERT(mPPOSceneSetLayout != VK_NULL_HANDLE && mDevice != VK_NULL_HANDLE && "Material system context not set!");

		SHARD3D_ASSERT(dataLayout != VK_NULL_HANDLE && "No data layout provided!");
		SHARD3D_ASSERT(pipelineLayout != nullptr && "No pipeline layout provided!");
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			mPPOSceneSetLayout,
			dataLayout
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(mDevice->device(), &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}

	void MaterialHandler::createPPOPipeline(uPtr<ComputePipeline>* pipeline, VkPipelineLayout pipelineLayout, const std::string& compute_shader) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		*pipeline = make_uPtr<ComputePipeline>(
			*mDevice,
			pipelineLayout,
			compute_shader
			);
	}
	void MaterialHandler::createPPOPipeline(uPtr<ComputePipeline>* pipeline, VkPipelineLayout pipelineLayout, const std::vector<char>& compute_code) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
		if (compute_code.empty()) {
			SHARD3D_ERROR("Shader compiling for post processing material 'unknown' failed!!");
			return;
		}
		*pipeline = make_uPtr<ComputePipeline>(
			*mDevice,
			pipelineLayout,
			compute_code
			);
	}
	void MaterialHandler::destroyPipelineLayout(VkPipelineLayout pipelineLayout) {
		vkDestroyPipelineLayout(mDevice->device(), pipelineLayout, nullptr);
	}
	void MaterialHandler::bindMaterialClassDeferred(SurfaceMaterialClassOptionsFlags flags, VkCommandBuffer commandBuffer) {
		materialClassesDeferred[flags]->bindDeferred(commandBuffer);
	}
	void MaterialHandler::bindMaterialClassDeferredLighting(VkCommandBuffer commandBuffer, VkDescriptorSet globalSet) {
		SHARD3D_ASSERT(materialClassDeferredLighting->materialPipelineConfig.shaderPipelineLayout && "Cannot bind material if shader pipeline was never created!");

		VkDescriptorSet sets[2]{
			globalSet,
			MaterialHandler::deferredRenderingSystem->getGBufferAttachmentSubpassInputDescriptorSet()
		};

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			materialClassDeferredLighting->materialPipelineConfig.shaderPipelineLayout,
			0,
			2,
			sets,
			0,
			nullptr
		);

		materialClassDeferredLighting->bindDeferredLighting(commandBuffer);
	}
	void MaterialHandler::bindMaterialClassForward(SurfaceMaterialClassOptionsFlags flags, VkCommandBuffer commandBuffer) {
		materialClassesForward[flags]->bindForward(commandBuffer);
	}
	SurfaceMaterialClass* MaterialHandler::getMaterialClass(SurfaceMaterialClassOptionsFlags flags) {
		return materialClassesGlobal[flags];
	}

	void MaterialHandler::debugPrintFlags(SurfaceMaterialClassOptionsFlags flags) {
		if (flags & SurfaceMaterialClassOptions_Shaded) 
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_Shaded));
		if (flags & SurfaceMaterialClassOptions_Unshaded)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_Unshaded));
		if (flags & SurfaceMaterialClassOptions_ShadedClearCoat)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_ShadedClearCoat));
		if (flags & SurfaceMaterialClassOptions_Opaque)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_Opaque));
		if (flags & SurfaceMaterialClassOptions_Masked)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_Masked));
		if (flags & SurfaceMaterialClassOptions_Translucent)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_Translucent));
		if (flags & SurfaceMaterialClassOptions_VirtualUber)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_VirtualUber));
		if (flags & SurfaceMaterialClassOptions_FrontfaceCulling)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_FrontfaceCulling));
		if (flags & SurfaceMaterialClassOptions_NoCulling)
			SHARD3D_LOG("Flag has: {0}", TOSTRING(SurfaceMaterialClassOptions_NoCulling));
	}

	void SurfaceMaterialClassForward::bindForward(VkCommandBuffer commandBuffer) {
		SHARD3D_ASSERT(options_flags & SurfaceMaterialClassOptions_Translucent && "Cannot bind a forward shader to a non transparent class with in deferred rendering!");
		SHARD3D_ASSERT(this->materialPipelineConfig.shaderPipelineLayout && "Cannot bind material if shader pipeline was never created!");

		materialPipelineConfig.shaderPipeline->bind(commandBuffer);
	}
	void SurfaceMaterialClassDeferred::bindDeferred(VkCommandBuffer commandBuffer) {
		SHARD3D_ASSERT(options_flags &~ SurfaceMaterialClassOptions_Translucent && "Cannot bind a transparent material to a deferred rendering pass!");
		SHARD3D_ASSERT(this->materialPipelineConfig.shaderPipelineLayout && "Cannot bind material if shader pipeline was never created!");
		
		materialPipelineConfig.shaderPipeline->bind(commandBuffer);
	}
	void SurfaceMaterialClassDeferredLighting::bindDeferredLighting(VkCommandBuffer commandBuffer) {
		materialPipelineConfig.shaderPipeline->bind(commandBuffer);
	}

	SurfaceMaterialClass::SurfaceMaterialClass(EngineDevice& device, SurfaceMaterialClassOptionsFlags flags) : engineDevice(device), options_flags(flags) {}
	SurfaceMaterialClass::~SurfaceMaterialClass() {
		vkDestroyPipelineLayout(engineDevice.device(), materialPipelineConfig.shaderPipelineLayout, nullptr);
	}
//
// FORWARD
//
	SurfaceMaterialClassForward::SurfaceMaterialClassForward(EngineDevice& device, SurfaceMaterialClassOptionsFlags flags) : SurfaceMaterialClass(device, flags) {
		materialDescriptorLayoutInfo.factorLayout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		if (options_flags & ~SurfaceMaterialClassOptions_VirtualUber) {
			EngineDescriptorSetLayout::Builder textureLayout_builder = EngineDescriptorSetLayout::Builder(engineDevice);
			textureLayout_builder.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			if (options_flags & SurfaceMaterialClassOptions_Masked)
				textureLayout_builder.addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

			materialDescriptorLayoutInfo.textureLayout = textureLayout_builder.build();
		}
		else {
			EngineDescriptorSetLayout::Builder textureLayout_builder = EngineDescriptorSetLayout::Builder(engineDevice);
			for (int i = 0; i < MAX_SUPPORTED_SAMPLER2D_COUNT_VIRTUAL_UBER; i++)
				textureLayout_builder.addBinding(i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

			materialDescriptorLayoutInfo.textureLayout = textureLayout_builder.build();
		}
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128; // sizeof(MeshPushConstantData)

		materialPipelineConfig = _MaterialGraphicsPipelineConfigInfo();
		MaterialHandler::createSurfacePipelineLayout(
			&materialPipelineConfig.shaderPipelineLayout,
			{ MaterialHandler::getDeferredRenderer()->getGBufferAttachmentSubpassInputLayout()->getDescriptorSetLayout(), materialDescriptorLayoutInfo.factorLayout->getDescriptorSetLayout(), materialDescriptorLayoutInfo.textureLayout->getDescriptorSetLayout() },
			&pushConstantRange
		);
		GraphicsPipelineConfigInfo pipelineConfigInfo{};
		GraphicsPipeline::pipelineConfig(pipelineConfigInfo)
			.defaultGraphicsPipelineConfigInfo()
			.enableVertexDescriptions()
			.enableAlphaBlending(VK_BLEND_OP_ADD)
			.disableDepthWrite()
			.setSubpass(1);

		if (options_flags & SurfaceMaterialClassOptions_NoCulling)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).setCullMode(VK_CULL_MODE_NONE);
		if (options_flags & SurfaceMaterialClassOptions_FrontfaceCulling)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).setCullMode(VK_CULL_MODE_BACK_BIT);


		MaterialHandler::createSurfacePipeline(
			&materialPipelineConfig.shaderPipeline,
			materialPipelineConfig.shaderPipelineLayout,
			pipelineConfigInfo,
			this, 2);
	}

//
// DEFERRED
//
	SurfaceMaterialClassDeferred::SurfaceMaterialClassDeferred(EngineDevice& device, SurfaceMaterialClassOptionsFlags flags) : SurfaceMaterialClass(device, flags) {
		materialDescriptorLayoutInfo.factorLayout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		if (options_flags & ~SurfaceMaterialClassOptions_VirtualUber) {
			EngineDescriptorSetLayout::Builder textureLayout_builder = EngineDescriptorSetLayout::Builder(engineDevice);
			textureLayout_builder.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			if (options_flags & SurfaceMaterialClassOptions_Masked)
				textureLayout_builder.addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			
			materialDescriptorLayoutInfo.textureLayout = textureLayout_builder.build();
		}
		else {
			EngineDescriptorSetLayout::Builder textureLayout_builder = EngineDescriptorSetLayout::Builder(engineDevice);
			for (int i = 0; i < MAX_SUPPORTED_SAMPLER2D_COUNT_VIRTUAL_UBER; i++)
				textureLayout_builder.addBinding(i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

			materialDescriptorLayoutInfo.textureLayout = textureLayout_builder.build();
		}
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128; // sizeof(MeshPushConstantData)

		materialPipelineConfig = _MaterialGraphicsPipelineConfigInfo();
		MaterialHandler::createSurfacePipelineLayout(
			&materialPipelineConfig.shaderPipelineLayout,
			{ materialDescriptorLayoutInfo.factorLayout->getDescriptorSetLayout(), materialDescriptorLayoutInfo.textureLayout->getDescriptorSetLayout() },
			&pushConstantRange
		);
		GraphicsPipelineConfigInfo pipelineConfigInfo{};
		GraphicsPipeline::pipelineConfig(pipelineConfigInfo)
			.defaultGraphicsPipelineConfigInfo()
			.enableVertexDescriptions()
			.setSubpass(0);
		if (options_flags & SurfaceMaterialClassOptions_NoCulling)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).setCullMode(VK_CULL_MODE_NONE);
		if (options_flags & SurfaceMaterialClassOptions_FrontfaceCulling)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).setCullMode(VK_CULL_MODE_BACK_BIT);

		pipelineConfigInfo.colorBlendInfo.attachmentCount = 3;
		VkPipelineColorBlendAttachmentState attachments[3]{ pipelineConfigInfo.colorBlendAttachment,pipelineConfigInfo.colorBlendAttachment ,pipelineConfigInfo.colorBlendAttachment };
		pipelineConfigInfo.colorBlendInfo.pAttachments = attachments;

		MaterialHandler::createSurfacePipeline(
			&materialPipelineConfig.shaderPipeline,
			materialPipelineConfig.shaderPipelineLayout,
			pipelineConfigInfo,
			this, 0);

	}

//
// DEFERRED LIGHTING
//
	SurfaceMaterialClassDeferredLighting::SurfaceMaterialClassDeferredLighting(EngineDevice& device) : engineDevice(device) {
		materialPipelineConfig = _MaterialGraphicsPipelineConfigInfo();
		MaterialHandler::createSurfacePipelineLayout(
			&materialPipelineConfig.shaderPipelineLayout,
			{ MaterialHandler::deferredRenderingSystem->getGBufferAttachmentSubpassInputLayout()->getDescriptorSetLayout() }
		);
		GraphicsPipelineConfigInfo pipelineConfigInfo{};
		GraphicsPipeline::pipelineConfig(pipelineConfigInfo)
			.defaultGraphicsPipelineConfigInfo()
			.disableDepthTest()
			.setCullMode(VK_CULL_MODE_BACK_BIT)
			.setSubpass(1);

		std::stringstream stream;
		std::stringstream cacheDir;

		cacheDir << "assets/shaderdata/materials/surface_deferredlighting";
		cacheDir << ".spv";
		stream << IOUtils::readText("assets/_engine/shaderdata/surface_material/surface_material_deferred_lighting.frag");

		std::string finalString = stream.rdbuf()->str();
		finalString = finalString.substr(0, finalString.length() - 2);

		std::vector<char> myBytes = ShaderSystem::compileOnTheFlyDirect(finalString, cacheDir.str().c_str(), ShaderType::Pixel);
		if (!myBytes.empty()) {
			IOUtils::writeStackBinary(myBytes.data(), myBytes.size(), cacheDir.str());
		}
		else {
			SHARD3D_ERROR("Shader compiling for material class '{0}' failed!! Reverting to previous best shader", cacheDir.str());
		}
		MaterialHandler::compiledShaders.push_back(cacheDir.str());
		MaterialHandler::createSurfacePipeline(
			&materialPipelineConfig.shaderPipeline,
			materialPipelineConfig.shaderPipelineLayout,
			pipelineConfigInfo,
			cacheDir.str(), "assets/shaderdata/fullscreen_quad_nouv.vert.spv");
	}
	SurfaceMaterialClassDeferredLighting::~SurfaceMaterialClassDeferredLighting() {
		vkDestroyPipelineLayout(engineDevice.device(), materialPipelineConfig.shaderPipelineLayout, nullptr);
	}
}
