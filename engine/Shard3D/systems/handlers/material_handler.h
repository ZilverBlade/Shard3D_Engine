#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"
#include "../../core/vulkan_api/pipeline_compute.h"
namespace Shard3D {
	namespace Components {
		struct TransformComponent;
		struct Mesh3DComponent;
	}

	/*
		**current system:**
			GOOD | bind mesh once
			GOOD | iterate over all meshes once per frame
			BAD  | bind material pipeline multiple times

		**material class draw vectors:**
			GOOD | no iterations
			GOOD | bind pipeline for each material class once per frame
			BAD  | bind mesh multiple times per frame

		**material category vectors per submesh slot per mesh:**
			GOOD | bind mesh once
			GOOD | bind pipeline for each material class once per frame
			BAD  | iterate over meshes X amount of times, of which X is the amount of material class combinations

	>>	**material category vectors per submesh slot per mesh in master rendering vector:**
	>>		MEH  | bind mesh the amount times the submeshes have unique material classes
	>>		GOOD | bind pipeline for each material class once per frame
	>>		GOOD | iterate over meshes X amount of times, of which X is the amount of unique material classes per mesh (mesh binding is done the least amount of times required)
*/

	enum SurfaceMaterialClassOptions {
		SurfaceMaterialClassOptions_Shaded = BIT(0),	
		SurfaceMaterialClassOptions_Unshaded = BIT(1),			// unused
		SurfaceMaterialClassOptions_ShadedClearCoat = BIT(2),	// unused
		SurfaceMaterialClassOptions_Opaque = BIT(3),
		SurfaceMaterialClassOptions_Masked = BIT(4),
		SurfaceMaterialClassOptions_Translucent = BIT(5),
		SurfaceMaterialClassOptions_VirtualUber = BIT(6),		// unused
		SurfaceMaterialClassOptions_FrontfaceCulling = BIT(7),	
		SurfaceMaterialClassOptions_NoCulling = BIT(8),
	};
	typedef uint32_t SurfaceMaterialClassOptionsFlags;

	struct _MaterialGraphicsPipelineConfigInfo {
		VkPipelineLayout shaderPipelineLayout{};
		uPtr<GraphicsPipeline> shaderPipeline{};
	};
	struct _MaterialComputePipelineConfigInfo {
		VkPipelineLayout shaderPipelineLayout{};
		uPtr<ComputePipeline> shaderPipeline{};
	};
	struct _SurfaceMaterialShaderDescriptorLayoutsInfo {
		uPtr<EngineDescriptorSetLayout> factorLayout{};
		uPtr<EngineDescriptorSetLayout> textureLayout{};
	};
	struct _DrawData {
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL; // will likely just be fill all the time
		VkCullModeFlags culling = VK_CULL_MODE_FRONT_BIT;
	};
	
	class SurfaceMaterialClass {
	public:
		DELETE_COPY(SurfaceMaterialClass)

		SurfaceMaterialClass(EngineDevice& device, SurfaceMaterialClassOptionsFlags flags);
		void bind(VkCommandBuffer commandBuffer);
		VkPipelineLayout getPipelineLayout() { return materialPipelineConfig.shaderPipelineLayout; }
		SurfaceMaterialClassOptionsFlags getClass() { return options_flags; }
		_SurfaceMaterialShaderDescriptorLayoutsInfo& getDescriptorLayouts() { return materialDescriptorLayoutInfo; }
	private:
		_MaterialGraphicsPipelineConfigInfo materialPipelineConfig{};
		_SurfaceMaterialShaderDescriptorLayoutsInfo materialDescriptorLayoutInfo{};
		SurfaceMaterialClassOptionsFlags options_flags;
		EngineDevice& engineDevice;
	};

	struct SurfaceMaterialRenderInfo {
		Components::TransformComponent* transform;
		Components::Mesh3DComponent* mesh;
		std::vector<uint32_t> material_indices;
	};

	class SurfaceMaterial;
	class MaterialHandler {
	public:
		static void setCurrentDevice(EngineDevice& device) { mDevice = &device; }
		static void setRenderPassContext(VkRenderPass renderPass) { mRenderPass = renderPass; }
		static void setGlobalSetLayout(VkDescriptorSetLayout globalSetLayout) { mGlobalSetLayout = globalSetLayout; }
		static void setRenderedSceneImageLayout(VkDescriptorSetLayout renderedSceneLayout) { mPPOSceneSetLayout = renderedSceneLayout; }
		static void setAllAvailableMaterialShaderPermutations(std::vector<SurfaceMaterialClassOptionsFlags>&& shaders);
		static void destroy();

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

		static void createSurfacePipeline(uPtr<GraphicsPipeline>* pipeline, VkPipelineLayout pipelineLayout, GraphicsPipelineConfigInfo& pipelineConfig, SurfaceMaterialClass* self);
		
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

		// RENDERING LIST
		static void bindMaterialClass(SurfaceMaterialClassOptionsFlags flags, VkCommandBuffer commandBuffer);
		static SurfaceMaterialClass* getMaterialClass(SurfaceMaterialClassOptionsFlags flags);
		static auto& getAvailableClasses() { return materialClassesOptions; }
		static void debugPrintFlags(SurfaceMaterialClassOptionsFlags flags);
	private:
		static inline EngineDevice* mDevice;
		static inline VkRenderPass mRenderPass{};
		static inline VkDescriptorSetLayout mGlobalSetLayout{};
		static inline VkDescriptorSetLayout mPPOSceneSetLayout{};
		static inline std::vector<std::string> compiledShaders{};
																	// unordered map instead of vector to speed up removing/adding elements if necessary on the fly
		static inline std::vector<SurfaceMaterialClassOptionsFlags> materialClassesOptions;
		static inline hashMap<SurfaceMaterialClassOptionsFlags, SurfaceMaterialClass*> materialClasses;
	};
}
