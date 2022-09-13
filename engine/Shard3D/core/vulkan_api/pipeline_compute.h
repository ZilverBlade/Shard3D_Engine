#pragma once
#include "device.h"

namespace Shard3D {
	class ComputePipeline {
	public:
		ComputePipeline(EngineDevice& device, VkPipelineLayout& pipelineLayout, const std::string& shaderFilePath);
		ComputePipeline(EngineDevice& device, VkPipelineLayout& pipelineLayout, const std::vector<char>& shaderData);
		~ComputePipeline();

		ComputePipeline(const ComputePipeline&) = delete;
		ComputePipeline& operator=(const ComputePipeline&) = delete;
		ComputePipeline() = default;

		void bindCompute(VkCommandBuffer commandBuffer);

		void destroyComputePipeline();

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createComputePipeline(
			VkPipelineLayout& pipelineLayout,
			std::vector<char> shaderCode
		);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		EngineDevice& engineDevice;
		VkPipeline computePipeline;
		VkShaderModule computeShaderModule;
	};
}