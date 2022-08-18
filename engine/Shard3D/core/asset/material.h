#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"

/*
* Shard 3D Material system
* 
* Object oriented design.
* 
* 
*/

/*
* Material shaders:
*	Surface:
*		- surface_shaded_opaque_shader
*		- surface_shaded_masked_shader
*		- surface_shaded_translucent_shader
* 		- surface_shaded_translucent_masked_shader
* 
*		- surface_unshaded_opaque_shader
*		- surface_unshaded_masked_shader
*		- surface_unshaded_translucent_shader
* 		- surface_unshaded_translucent_masked_shader
*
*/
namespace Shard3D {
	struct DrawData {
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlags culling = VK_CULL_MODE_NONE;
	};
	struct SurfaceMaterialShaderConfigInfo {
		VkDescriptorSet factors{};
		VkDescriptorSet textures{};

		uPtr<EngineDescriptorSetLayout> factorLayout{};
		uPtr<EngineDescriptorSetLayout> imageLayout{};
	};

	struct MaterialPipelineConfigInfo {
		VkPipelineLayout pipelineLayout{};
		uPtr<EnginePipeline> shaderPipeline{};
	};

	/*
	*	Material that can be used on meshes.
	*	Can have specular, shininness and metallic properties, texture maps, masked, and be transparent.
	*/ 
	class SurfaceMaterial {
	public:
		std::string materialTag = "Some kind of material";

		DrawData drawData{};

		virtual void createMaterialDescriptors(uPtr<EngineDescriptorPool>& descriptorPool) = 0;

		void bind(VkCommandBuffer commandBuffer);
	protected:

		MaterialPipelineConfigInfo* materialPipelineConfig;
		SurfaceMaterialShaderConfigInfo materialDescriptorInfo;
	};	

	// SSMR (Specular/Shininess/Metallic Rendering), Opaque
	class SurfaceMaterial_ShadedOpaque : public SurfaceMaterial {
	public:
		std::string normalTex = ENGINE_NRMTEX;

		glm::vec4 emissiveColor{ 0.f };
		std::string emissiveTex = ENGINE_WHTTEX;

		glm::vec4 diffuseColor{ 1.f };
		std::string diffuseTex = ENGINE_ERRTEX;

		float specular = 0.5f;
		std::string specularTex = ENGINE_WHTTEX;

		float shininess = 0.5f;
		std::string shininessTex = ENGINE_WHTTEX;

		float metallic = 0.f;
		std::string metallicTex = ENGINE_WHTTEX;

		static void setupMaterialShaderPipeline(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		void createMaterialDescriptors(uPtr<EngineDescriptorPool>& descriptorPool) override;
	private:					
		static inline MaterialPipelineConfigInfo surfaceShadedOpaque;
	};

	/*
	*	Material that can be used for projecting textures onto meshes.
	*	It will overlay whatever previous material with it's properties and render over it.
	*/
	class DecalMaterial {
	public:
		virtual void createMaterialDescriptors(EngineDescriptorPool& descriptorPool) = 0;

		void bind(VkCommandBuffer commandBuffer);
	};

	/*
	*	Material that can be used for post processing effects.
	*	As this is quite complex, you must write shader files for this.
	*/
	class PostProcessingMaterial {
	public:
		virtual volatile void setupMaterialShaderPipeline(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

		void bind(VkCommandBuffer commandBuffer);
	};

}