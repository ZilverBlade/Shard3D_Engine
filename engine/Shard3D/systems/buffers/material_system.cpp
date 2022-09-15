#include "../../s3dpch.h" 

#include "material_system.h"
#include "../../core/asset/assetmgr.h"
#include <fstream>
#include "../computational/shader_system.h"

namespace Shard3D {
	void MaterialSystem::recompileSurface() {
		compiledShaders.clear();
		for (auto& [key, material] : ResourceHandler::getSurfaceMaterialAssets()) {
			ResourceHandler::rebuildSurfaceMaterial(material);
		}
	}
	void MaterialSystem::recompilePPO() {
		for (auto& [key, material] : ResourceHandler::getPPOMaterialAssets()) {
			ResourceHandler::rebuildPPOMaterial(material);
		}
	}

	void MaterialSystem::createSurfacePipelineLayout(VkPipelineLayout* pipelineLayout, VkDescriptorSetLayout factorLayout, VkDescriptorSetLayout textureLayout) {
		SHARD3D_ASSERT(mRenderPass != VK_NULL_HANDLE && mGlobalSetLayout != VK_NULL_HANDLE && mDevice != VK_NULL_HANDLE && "Material system context not set!");

		SHARD3D_ASSERT(pipelineLayout != nullptr && "No pipeline layout provided!");
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			mGlobalSetLayout,
			factorLayout,
			textureLayout
		};

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
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
	void MaterialSystem::createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, GraphicsPipelineConfigInfo& pipelineConfig, const std::string& fragment_shader) {
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
	void MaterialSystem::createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, GraphicsPipelineConfigInfo& pipelineConfig, SurfaceMaterial* self) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		pipelineConfig.renderPass = mRenderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		std::stringstream stream;
		std::stringstream cacheDir;
		cacheDir << "assets/shaderdata/materials/surface_";
		stream << "#version 450\n";
		if (dynamic_cast<SurfaceMaterial_Shaded*>(self)) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_SHADED\n";
			cacheDir << "shaded_";
		} if (self->getBlendMode() & SurfaceMaterialBlendModeMasked) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_MASKED\n";
			cacheDir << "masked_";
		} if (self->getBlendMode() & SurfaceMaterialBlendModeTranslucent) {
			stream << "#define S3DSDEF_SHADER_PERMUTATION_SURFACE_TRANSLUCENT\n";
			cacheDir << "translucent_";
		} if (self->getBlendMode() == 0)
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


	void MaterialSystem::createPPOPipelineLayout(VkPipelineLayout* pipelineLayout, VkDescriptorSetLayout dataLayout) {
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

	void MaterialSystem::createPPOPipeline(uPtr<ComputePipeline>* pipeline, VkPipelineLayout pipelineLayout, const std::string& compute_shader) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		*pipeline = make_uPtr<ComputePipeline>(
			*mDevice,
			pipelineLayout,
			compute_shader
		);
	}
	void MaterialSystem::createPPOPipeline(uPtr<ComputePipeline>* pipeline, VkPipelineLayout pipelineLayout, const std::vector<char>& compute_code) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		*pipeline = make_uPtr<ComputePipeline>(
			*mDevice,
			pipelineLayout,
			compute_code
		);
	}
	void MaterialSystem::destroyPipelineLayout(VkPipelineLayout pipelineLayout) {
		vkDestroyPipelineLayout(mDevice->device(), pipelineLayout, nullptr);	
	}
}
