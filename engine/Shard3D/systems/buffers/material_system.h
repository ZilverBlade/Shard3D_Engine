#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"
#include "../../core/vulkan_api/pipeline_compute.h"
namespace Shard3D {
	class MaterialSystem {
	public:
		static void setCurrentDevice(EngineDevice& device) { mDevice = &device; }
		static void setRenderPassContext(VkRenderPass renderPass) { mRenderPass = renderPass; }
		static void setGlobalSetLayout(VkDescriptorSetLayout globalSetLayout) { mGlobalSetLayout = globalSetLayout; }
		static void setRenderedSceneImageLayout(VkDescriptorSetLayout renderedSceneLayout) { mPPOSceneSetLayout = renderedSceneLayout; }

		static void createSurfacePipelineLayout(
			VkPipelineLayout* pipelineLayout,
			VkDescriptorSetLayout factorLayout,
			VkDescriptorSetLayout textureLayout
		);

		static void createSurfacePipeline(
			uPtr<GraphicsPipeline>* pipeline,
			VkPipelineLayout pipelineLayout,
			GraphicsPipelineConfigInfo& pipelineConfig,
			const std::string& fragment_shader
		);
		
		static void createPPOPipelineLayout(
			VkPipelineLayout* pipelineLayout,
			VkDescriptorSetLayout dataLayout
		);

		static void createPPOPipeline(
			uPtr<ComputePipeline>* pipeline,
			VkPipelineLayout pipelineLayout,
			const std::string& compute_shader
		);

		static void destroyPipelineLayout(VkPipelineLayout pipelineLayout);
	private:
		static inline EngineDevice* mDevice;
		static inline VkRenderPass mRenderPass{};
		static inline VkDescriptorSetLayout mGlobalSetLayout{};
		static inline VkDescriptorSetLayout mPPOSceneSetLayout{};
	};
}
