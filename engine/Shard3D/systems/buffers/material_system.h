#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"
#include "../../core/vulkan_api/pipeline_compute.h"
namespace Shard3D {
	enum SurfaceMaterialClassOptions {
		SurfaceMaterialClassOptions_Shaded = BIT(0),
		SurfaceMaterialClassOptions_Unshaded = BIT(1),
		SurfaceMaterialClassOptions_Masked = BIT(2),
		SurfaceMaterialClassOptions_Translucent = BIT(3),
		SurfaceMaterialClassOptions_VirtualUber = BIT(4),
		SurfaceMaterialClassOptions_FrontfaceCulling = BIT(5),
		SurfaceMaterialClassOptions_NoCulling = BIT(6),
	};

	/*
		**current system:**
			GOOD | bind mesh once
			GOOD | iterate over all meshes once per frame
			BAD  | bind pipeline multiple times
		
		**material class draw vectors:**
			GOOD | no iterations
			GOOD | bind pipeline for each material class once per frame
			BAD  | bind mesh multiple times per frame
		
		**material category vectors per submesh slot per mesh:**
			GOOD | bind mesh once
			GOOD | bind pipeline for each material class once per frame
			BAD  | iterate over meshes X amount of times, of which X is the amount of material class combinations
	*/

	struct SurfaceMaterialRenderInfo {
		glm::mat4 modelMatrix, normalMatrix;
		AssetID material;
		AssetID mesh;
	};

typedef uint32_t SurfaceMaterialClassOptionsFlags;

	class SurfaceMaterial;
	class MaterialSystem {
	public:
		static void setCurrentDevice(EngineDevice& device) { mDevice = &device; }
		static void setRenderPassContext(VkRenderPass renderPass) { mRenderPass = renderPass; }
		static void setGlobalSetLayout(VkDescriptorSetLayout globalSetLayout) { mGlobalSetLayout = globalSetLayout; }
		static void setRenderedSceneImageLayout(VkDescriptorSetLayout renderedSceneLayout) { mPPOSceneSetLayout = renderedSceneLayout; }

		static void recompileSurface();
		static void recompilePPO();

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

		static void createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, GraphicsPipelineConfigInfo& pipelineConfig, SurfaceMaterial* self);
		
		static void createPPOPipelineLayout(
			VkPipelineLayout* pipelineLayout,
			VkDescriptorSetLayout dataLayout
		);

		static void createPPOPipeline(
			uPtr<ComputePipeline>* pipeline,
			VkPipelineLayout pipelineLayout,
			const std::string& compute_shader
		);

		static void createPPOPipeline(uPtr<ComputePipeline>* pipeline, VkPipelineLayout pipelineLayout, const std::vector<char>& compute_code);

		static void destroyPipelineLayout(VkPipelineLayout pipelineLayout);
	private:
		static inline EngineDevice* mDevice;
		static inline VkRenderPass mRenderPass{};
		static inline VkDescriptorSetLayout mGlobalSetLayout{};
		static inline VkDescriptorSetLayout mPPOSceneSetLayout{};
		static inline std::vector<std::string> compiledShaders{};

		static inline hashMap<SurfaceMaterialClassOptionsFlags, std::vector<AssetID>> materialRendering;
	};
}
