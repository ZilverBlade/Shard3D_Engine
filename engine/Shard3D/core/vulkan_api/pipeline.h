#pragma once
#include "device.h"

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
		struct _pipeline_cfg {
			_pipeline_cfg(PipelineConfigInfo& _configInfo) : configInfo(_configInfo) { }

			_pipeline_cfg defaultPipelineConfigInfo();
			_pipeline_cfg enableVertexDescriptions();
			_pipeline_cfg enableAlphaBlending(VkBlendOp blendOp = VK_BLEND_OP_ADD);
			_pipeline_cfg lineRasterizer(float thickness = 1.0f);
			_pipeline_cfg setCullingMode(VkCullModeFlags cullMode);
			_pipeline_cfg discardRasterizer();
			_pipeline_cfg forceSampleCount(VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
		private:
			PipelineConfigInfo& configInfo;
		};
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

		static _pipeline_cfg pipelineConfig(PipelineConfigInfo& _configInfo) { return _pipeline_cfg(_configInfo); }
		void destroyGraphicsPipeline();

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(
			const std::string& vertFilePath,
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo
		);

		void createComputePipeline(
			VkPipelineLayout& pipelineLayout,
			const std::string& shaderFilePath,
			const PipelineConfigInfo& configInfo
		);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		EngineDevice& engineDevice;
		VkPipeline graphicsPipeline;
		VkPipeline computePipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		VkShaderModule computeShaderModule;
	};
}