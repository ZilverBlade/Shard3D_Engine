#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"

namespace Shard3D {
	class MaterialSystem {
	public:
		static void setRenderPassContext(VkRenderPass renderPass) { mRenderPass = renderPass; }
		static void setGlobalSetLayout(VkDescriptorSetLayout globalSetLayout) { mGlobalSetLayout = globalSetLayout; }

		static void createSurfacePipelineLayout(
			EngineDevice& device,
			VkDescriptorSetLayout factorLayout,
			VkDescriptorSetLayout textureLayout,
			VkPipelineLayout* pipelineLayout
		);

		static void createSurfacePipeline(
			EngineDevice& device,
			uPtr<EnginePipeline>* pipeline,
			VkPipelineLayout pipelineLayout,
			PipelineConfigInfo& pipelineConfig,
			const std::string& fragment_shader
		);

	private:
		static inline VkRenderPass mRenderPass{};
		static inline VkDescriptorSetLayout mGlobalSetLayout{};
	};
}
