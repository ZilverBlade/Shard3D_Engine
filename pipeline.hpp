#pragma once

#include "device.hpp"

#include <string>
#include <vector>
namespace Shard3D {

	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class EnginePipeline {
	public:
		EnginePipeline(
			EngineDevice &device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo,
			bool recreate = false
		);
		EnginePipeline(
			std::string trash,
			EngineDevice& device,
			VkShaderStageFlagBits shaderStageFlag,
			VkPipelineLayout& pipelineLayout,
			const std::string& shaderFilePath,
			const PipelineConfigInfo& configInfo,
			bool recreate = false
		);
		~EnginePipeline();

		EnginePipeline(const EnginePipeline&) = delete;
		EnginePipeline& operator=(const EnginePipeline&) = delete;
		EnginePipeline() = default;

		void bind(VkCommandBuffer commandBuffer);
		void bindCompute(VkCommandBuffer commandBuffer);

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
		static void enableAlphaBlending(PipelineConfigInfo& configInfo, VkBlendOp blendOp);

		void destroyGraphicsPipeline();

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(
			const std::string& vertFilePath,
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo
		);

		void createComputeGraphicsPipeline(
			VkPipelineLayout& pipelineLayout,
			const std::string& shaderFilePath,
			const PipelineConfigInfo& configInfo
		);


		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		EngineDevice& engineDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		VkShaderModule computeShaderModule;
	};
}