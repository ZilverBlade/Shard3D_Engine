#include "../../s3dpch.h"  

#include "pipeline_compute.h"
#include "../asset/model.h"
#include <fstream>
#include "../misc/graphics_settings.h"
namespace Shard3D {

	ComputePipeline::ComputePipeline(
		EngineDevice& device,
		VkPipelineLayout& pipelineLayout,
		const std::string& shaderFilePath
	)
		: engineDevice{ device } {
		createComputePipeline(pipelineLayout, shaderFilePath);
	}

	ComputePipeline::~ComputePipeline() {
		vkDestroyShaderModule(engineDevice.device(), computeShaderModule, nullptr);
		vkDestroyPipeline(engineDevice.device(), computePipeline, nullptr);
	}

	std::vector<char> ComputePipeline::readFile(const std::string& filePath) {
		std::ifstream file{ filePath, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			SHARD3D_FATAL("failed to read shader: " + filePath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void ComputePipeline::createComputePipeline(
		VkPipelineLayout& pipelineLayout,
		const std::string& shaderFilePath
	) {
		auto shaderCode = readFile(shaderFilePath);

		VkPipelineShaderStageCreateInfo shaderStage{};
		createShaderModule(shaderCode, &computeShaderModule);
		shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStage.module = computeShaderModule;
		shaderStage.pName = "main";
		shaderStage.flags = 0;
		shaderStage.pNext = nullptr;
		shaderStage.pSpecializationInfo = nullptr;

		VkComputePipelineCreateInfo computePipelineCreateInfo {};
		computePipelineCreateInfo.stage = shaderStage;
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.layout = pipelineLayout;
		computePipelineCreateInfo.flags = 0;

		if (vkCreateComputePipelines(engineDevice.device(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &computePipeline) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create compute pipeline!");
		}
	}

	void ComputePipeline::destroyComputePipeline( 
	) {
		vkDestroyPipeline(engineDevice.device(), computePipeline, nullptr);
	}

	void ComputePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(engineDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to create shader module!");
		}
	}

	void ComputePipeline::bindCompute(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	}
}