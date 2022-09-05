#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"
#include "../asset/assetid.h"
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

namespace YAML {
	class Emitter;
	class Node;
}

namespace Shard3D {
	struct MaterialGraphicsPipelineConfigInfo {
		VkPipelineLayout shaderPipelineLayout{};
		uPtr<GraphicsPipeline> shaderPipeline{};
	};

	struct DrawData {
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlags culling = VK_CULL_MODE_NONE;
	};

	struct SurfaceMaterialShaderDescriptorInfo {
		VkDescriptorSet factors{};
		VkDescriptorSet textures{};

		uPtr<EngineDescriptorSetLayout> factorLayout{};
		uPtr<EngineDescriptorSetLayout> textureLayout{};
	};

	enum SurfaceMaterialBlendMode {
		SurfaceMaterialBlendModeOpaque = 0,
		SurfaceMaterialBlendModeMasked = BIT(0),
		SurfaceMaterialBlendModeTranslucent = BIT(1)
	};

#define SurfaceMaterialBlendMode_T uint32_t

	struct SurfaceMaterial_BlendModeMasked {
		AssetID maskTex = std::string(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX);
	};

	struct SurfaceMaterial_BlendModeTranslucent {
		AssetID opacityTex = std::string(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX);
		float opacity{ 1.f };
	};


	/*
	*	Material that can be used on meshes.
	*	Can have specular, shininness and metallic properties, texture maps, masked, and be transparent.
	*/ 
	class SurfaceMaterial {
	public:
		SurfaceMaterial();
		virtual ~SurfaceMaterial();

		SurfaceMaterial_BlendModeMasked* maskedInfo{};
		SurfaceMaterial_BlendModeTranslucent* translucentInfo{};

		uint32_t getBlendMode() { return blendMode; }
		void setBlendMode(SurfaceMaterialBlendMode_T mode);

		std::string materialTag = "Some kind of material";

		DrawData drawData{};

		virtual void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) = 0;
		virtual void serialize(YAML::Emitter* out) = 0;
		virtual void deserialize(YAML::Node* data) = 0;
		virtual void loadAllTextures() = 0;

		void bind(VkCommandBuffer commandBuffer, VkDescriptorSet globalSet);
		inline bool isBuilt() { return built; }
		VkPipelineLayout getPipelineLayout() { return materialPipelineConfig->shaderPipelineLayout; }
	protected:
		bool built = false;
		uPtr<MaterialGraphicsPipelineConfigInfo> materialPipelineConfig{};
		SurfaceMaterialShaderDescriptorInfo materialDescriptorInfo{};
		uPtr<EngineBuffer> factorsBuffer;
		SurfaceMaterialBlendMode_T blendMode = SurfaceMaterialBlendModeOpaque;
	};	

	// SSMR (Specular/Shininess/Metallic Rendering)
	class SurfaceMaterial_Shaded : public SurfaceMaterial {
	public:
		AssetID normalTex = std::string(ENGINE_NRMTEX ENGINE_ASSET_SUFFIX);

		glm::vec3 emissiveColor{ 0.f };
		AssetID emissiveTex = std::string(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX);

		glm::vec3 diffuseColor{ 1.f };
		AssetID diffuseTex = std::string(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX);

		float specular = 0.5f;
		AssetID specularTex = std::string(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX);

		float shininess = 0.5f;
		AssetID shininessTex = std::string(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX);

		float metallic = 0.f;
		AssetID metallicTex = std::string(ENGINE_WHTTEX ENGINE_ASSET_SUFFIX);

		void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) override;
		void serialize(YAML::Emitter* out) override;
		void deserialize(YAML::Node* data) override;
		void loadAllTextures() override;
	};

	/*
	*	Material that can be used for projecting textures onto meshes.
	*	It will overlay whatever previous material with it's properties and render over it.
	*/
	class DecalMaterial {
	public:
		virtual void createMaterialShader(EngineDevice& device, EngineDescriptorPool& descriptorPool) = 0;

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