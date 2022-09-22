#pragma once
#include "device.h"

namespace Shard3D {

	struct GraphicsPipelineConfigInfo {
		GraphicsPipelineConfigInfo() = default;
		GraphicsPipelineConfigInfo(const GraphicsPipelineConfigInfo&) = delete;
		GraphicsPipelineConfigInfo& operator=(const GraphicsPipelineConfigInfo&) = delete;

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

	class GraphicsPipeline {
		struct _pipeline_cfg {
			_pipeline_cfg(GraphicsPipelineConfigInfo& _configInfo) : configInfo(_configInfo) { }

			_pipeline_cfg defaultGraphicsPipelineConfigInfo();
			_pipeline_cfg enableVertexDescriptions();
			_pipeline_cfg enableAlphaBlending(VkBlendOp blendOp = VK_BLEND_OP_ADD);
			_pipeline_cfg lineRasterizer(float thickness = 1.0f);
			_pipeline_cfg setCullMode(VkCullModeFlags cullMode);
			_pipeline_cfg discardRasterizer();
			_pipeline_cfg forceSampleCount(VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
			_pipeline_cfg disableDepthTest();
			_pipeline_cfg disableDepthWrite();
			_pipeline_cfg setSubpass(uint32_t subpassIndex);
		private:
			GraphicsPipelineConfigInfo& configInfo;
		};
	public:
		GraphicsPipeline(
			EngineDevice &device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const GraphicsPipelineConfigInfo& configInfo
		);

		GraphicsPipeline(
			EngineDevice& device, 
			const std::string& vertFilePath,
			const std::vector<char>& fragData, 
			const GraphicsPipelineConfigInfo& configInfo
		);
		
		~GraphicsPipeline();

		GraphicsPipeline(const GraphicsPipeline&) = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
		GraphicsPipeline() = default;

		void bind(VkCommandBuffer commandBuffer);
		
		static _pipeline_cfg pipelineConfig(GraphicsPipelineConfigInfo& _configInfo) { return _pipeline_cfg(_configInfo); }
		void destroyGraphicsPipeline();

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(
			const std::vector<char>& vertCode,
			const std::vector<char>& fragCode,
			const GraphicsPipelineConfigInfo& configInfo
		);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		EngineDevice& engineDevice;
		VkPipeline graphicsPipeline;
		VkPipeline computePipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}