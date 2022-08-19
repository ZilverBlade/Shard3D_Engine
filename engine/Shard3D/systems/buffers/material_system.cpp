#include "../../s3dpch.h" 

#include "material_system.h"
#include "../../core/asset/assetmgr.h"
#include <fstream>

namespace Shard3D {
	void MaterialSystem::createSurfacePipelineLayout(EngineDevice& device, VkDescriptorSetLayout factorLayout, VkDescriptorSetLayout textureLayout, VkPipelineLayout* pipelineLayout) {
		SHARD3D_ASSERT(mGlobalSetLayout != VK_NULL_HANDLE && "Material system context not set!");
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
		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create pipeline layout!");
		}
	}
	void MaterialSystem::createSurfacePipeline(EngineDevice& device, uPtr<EnginePipeline>* pipeline, VkPipelineLayout pipelineLayout, PipelineConfigInfo& pipelineConfig, const std::string& fragment_shader) {
		SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		pipelineConfig.renderPass = mRenderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		*pipeline = make_uPtr<EnginePipeline>(
			device,
			"assets/shaderdata/mesh_shader.vert.spv",
			fragment_shader,
			pipelineConfig
		);
	}
}
