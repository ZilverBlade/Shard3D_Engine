#include "../../s3dpch.h" 

#include "material_handler.h"
#include "render_handler.h"
#include "../../core/asset/assetmgr.h"
#include <fstream>
#include "../computational/shader_system.h"
#define MAX_SUPPORTED_SAMPLER2D_COUNT_VIRTUAL_UBER 16
namespace Shard3D {
	void MaterialHandler::setAllAvailableMaterialShaderPermutations(std::vector<SurfaceMaterialClassOptionsFlags>&& shaders) {
		RenderHandler::materialRendering.reserve(shaders.size() * 2);
		for (uint32_t permutation_ : shaders) {
			{
				uint32_t permutation = permutation_ | SurfaceMaterialClassOptions_FrontfaceCulling;
				RenderHandler::materialRendering[permutation] = hashMap<UUID, SurfaceMaterialRenderInfo>();
				materialClasses[permutation] = new SurfaceMaterialClass(*mDevice, permutation);
				materialClassesOptions.push_back(permutation);
			} {
				uint32_t permutation = permutation_ | SurfaceMaterialClassOptions_NoCulling;
				RenderHandler::materialRendering[permutation] = hashMap<UUID, SurfaceMaterialRenderInfo>();
				materialClasses[permutation] = new SurfaceMaterialClass(*mDevice, permutation);
				materialClassesOptions.push_back(permutation);
			}
		}
	}
	void MaterialHandler::destroy() {
		for (auto& [flags, mas_materials] : RenderHandler::materialRendering) {
			mas_materials.clear();
		}
		RenderHandler::materialRendering.clear();
		for (auto& [_, item] : materialClasses) {
			delete item;
		}
	}
	void MaterialHandler::recompileSurface() {
		compiledShaders.clear();
		for (auto& [key, material] : ResourceHandler::getSurfaceMaterialAssets()) {
			ResourceHandler::rebuildSurfaceMaterial(material);
		}
	}
	void MaterialHandler::recompilePPO() {
		for (auto& [key, material] : ResourceHandler::getPPOMaterialAssets()) {
			ResourceHandler::rebuildPPOMaterial(material);
		}
	}

	void MaterialHandler::createSurfacePipelineLayout(VkPipelineLayout* pipelineLayout, VkDescriptorSetLayout factorLayout, VkDescriptorSetLayout textureLayout) {
		SHARD3D_ASSERT(mRenderPass != VK_NULL_HANDLE && mGlobalSetLayout != VK_NULL_HANDLE && mDevice != VK_NULL_HANDLE && "Material system context not set!");

		SHARD3D_ASSERT(pipelineLayout != nullptr && "No pipeline layout provided!");
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			mGlobalSetLayout,
			factorLayout,
			textureLayout
		};

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128; // sizeof(MeshPushConstantData)

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(mDevice->device(), &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}
	void MaterialHandler::createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, GraphicsPipelineConfigInfo& pipelineConfig, const std::string& fragment_shader) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		pipelineConfig.renderPass = mRenderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		*pipeline = make_uPtr<GraphicsPipeline>(
			*mDevice,
			"assets/shaderdata/mesh_shader.vert.spv",
			fragment_shader,
			pipelineConfig
			);
	}


	void MaterialHandler::createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, GraphicsPipelineConfigInfo& pipelineConfig, SurfaceMaterialClass* self) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		pipelineConfig.renderPass = mRenderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		std::stringstream stream;
		std::stringstream cacheDir;
		cacheDir << "assets/shaderdata/materials/surface_";
		stream << "#version 450\n";
#ifdef ENEXP_ENABLE_FORWARD_GBUFFER
		stream << "#define S3SDSEXP_ENABLE_FORWARD_GBUFFER\n";
#endif
		if (self->getClass() & SurfaceMaterialClassOptions_Shaded) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED\n";
			cacheDir << "shaded_";
		} if (self->getClass() & SurfaceMaterialClassOptions_Masked) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED\n";
			cacheDir << "masked_";
		} if (self->getClass() & SurfaceMaterialClassOptions_Translucent) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_TRANSLUCENT\n";
			cacheDir << "translucent_";
		} if (self->getClass() == 0)
			cacheDir << "opaque_";
		cacheDir << ".spv";



		if (std::find(compiledShaders.begin(), compiledShaders.end(), cacheDir.str()) != compiledShaders.end()) {
			SHARD3D_LOG("Reading cached shader '{0}'", cacheDir.str());
			goto pipelinecreate;
		}

		{		// comment out the version 
			stream << "//" << IOUtils::readText("assets/_engine/shaderdata/surface_material/surface_material.frag");
			std::string finalString = stream.rdbuf()->str();
			finalString = finalString.substr(0, finalString.length() - 2);

			std::vector<char> myBytes = ShaderSystem::compileOnTheFlyDirect(finalString, "assets/_engine/shaderdata/surface_material/surface_material.frag", ShaderType::Pixel);
			IOUtils::writeStackBinary(myBytes.data(), myBytes.size(), cacheDir.str());
			compiledShaders.push_back(cacheDir.str());
		}
	pipelinecreate:
		*pipeline = make_uPtr<GraphicsPipeline>(
			*mDevice,
			"assets/shaderdata/mesh_shader.vert.spv",
			cacheDir.str(),
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

		*pipeline = make_uPtr<ComputePipeline>(
			*mDevice,
			pipelineLayout,
			compute_code
			);
	}
	void MaterialHandler::destroyPipelineLayout(VkPipelineLayout pipelineLayout) {
		vkDestroyPipelineLayout(mDevice->device(), pipelineLayout, nullptr);
	}
	void MaterialHandler::bindMaterialClass(SurfaceMaterialClassOptionsFlags flags, VkCommandBuffer commandBuffer) {
		materialClasses[flags]->bind(commandBuffer);
	}
	SurfaceMaterialClass* MaterialHandler::getMaterialClass(SurfaceMaterialClassOptionsFlags flags) {
		return materialClasses[flags];
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

	SurfaceMaterialClass::SurfaceMaterialClass(EngineDevice& device, SurfaceMaterialClassOptionsFlags flags) : engineDevice(device), options_flags(flags) {
		{
			materialDescriptorLayoutInfo.factorLayout = EngineDescriptorSetLayout::Builder(device)
				.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();
		}
		if (flags & ~SurfaceMaterialClassOptions_VirtualUber) {
			EngineDescriptorSetLayout::Builder textureLayout_builder = EngineDescriptorSetLayout::Builder(device);
			textureLayout_builder.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			textureLayout_builder.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			if (flags & SurfaceMaterialClassOptions_Masked)
				textureLayout_builder.addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			if (flags & SurfaceMaterialClassOptions_Translucent)
				textureLayout_builder.addBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

			materialDescriptorLayoutInfo.textureLayout = textureLayout_builder.build();
		}
		else {
			EngineDescriptorSetLayout::Builder textureLayout_builder = EngineDescriptorSetLayout::Builder(device);
			for (int i = 0; i < MAX_SUPPORTED_SAMPLER2D_COUNT_VIRTUAL_UBER; i++)
				textureLayout_builder.addBinding(i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

			materialDescriptorLayoutInfo.textureLayout = textureLayout_builder.build();
		}

		materialPipelineConfig = _MaterialGraphicsPipelineConfigInfo();
		MaterialHandler::createSurfacePipelineLayout(
			&materialPipelineConfig.shaderPipelineLayout,
			materialDescriptorLayoutInfo.factorLayout->getDescriptorSetLayout(),
			materialDescriptorLayoutInfo.textureLayout->getDescriptorSetLayout()
		);
		GraphicsPipelineConfigInfo pipelineConfigInfo{};
		GraphicsPipeline::pipelineConfig(pipelineConfigInfo)
			.defaultGraphicsPipelineConfigInfo()
			.enableVertexDescriptions();

		if (flags & SurfaceMaterialClassOptions_Translucent)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).enableAlphaBlending(VK_BLEND_OP_ADD);
		if (flags & SurfaceMaterialClassOptions_Translucent)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).enableAlphaBlending(VK_BLEND_OP_ADD);

		if (flags & SurfaceMaterialClassOptions_NoCulling)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).setCullMode(VK_CULL_MODE_NONE);
		if (flags & SurfaceMaterialClassOptions_FrontfaceCulling)
			GraphicsPipeline::pipelineConfig(pipelineConfigInfo).setCullMode(VK_CULL_MODE_FRONT_BIT);

		// come back when gbuffer has been adjusted
#ifdef ENEXP_ENABLE_FORWARD_GBUFFER
		pipelineConfigInfo.colorBlendInfo.attachmentCount = 4;
		VkPipelineColorBlendAttachmentState attachments[4]{ pipelineConfigInfo.colorBlendAttachment, pipelineConfigInfo.colorBlendAttachment,pipelineConfigInfo.colorBlendAttachment ,pipelineConfigInfo.colorBlendAttachment };
		pipelineConfigInfo.colorBlendInfo.pAttachments = attachments;
#endif

		MaterialHandler::createSurfacePipeline(
			&materialPipelineConfig.shaderPipeline,
			materialPipelineConfig.shaderPipelineLayout,
			pipelineConfigInfo,
			this);
	}
	void SurfaceMaterialClass::bind(VkCommandBuffer commandBuffer) {
		materialPipelineConfig.shaderPipeline->bind(commandBuffer);
	}
}
