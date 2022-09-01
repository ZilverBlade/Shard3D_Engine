#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"

namespace Shard3D {
	class MaterialSystem {
	public:
		static void setRenderPassContext(VkRenderPass renderPass) { mRenderPass = renderPass; }
		static void setGlobalSetLayout(VkDescriptorSetLayout globalSetLayout) { mGlobalSetLayout = globalSetLayout; }
		static void setCurrentDevice(EngineDevice& device) { mDevice = &device; }
		static void createSurfacePipelineLayout(
			VkDescriptorSetLayout factorLayout,
			VkDescriptorSetLayout textureLayout,
			VkPipelineLayout* pipelineLayout
		);

		static void createSurfacePipeline(
			uPtr<GraphicsPipeline>* pipeline,
			VkPipelineLayout pipelineLayout,
			GraphicsPipelineConfigInfo& pipelineConfig,
			const std::string& fragment_shader
		);

		static void destroyPipelineLayout(VkPipelineLayout pipelineLayout);
	private:
		static inline EngineDevice* mDevice;
		static inline VkRenderPass mRenderPass{};
		static inline VkDescriptorSetLayout mGlobalSetLayout{};
	};
}
