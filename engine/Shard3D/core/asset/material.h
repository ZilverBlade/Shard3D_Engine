#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"
#include "../asset/assetid.h"
#include "../vulkan_api/pipeline_compute.h"
#include "../../systems/handlers/material_handler.h"
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

	enum class PPO_Types {
		Int32 = 2,
		Float = 3,
		Float2 = 4,
		Float4 = 6
	};

	// Helper function: 
	// Gets the enum type from a raw typeid name
	static PPO_Types _material_helper_getShaderEnumType(const char* typeID_raw) {
		if (typeID_raw == typeid(int).raw_name())		return PPO_Types::Int32;
		if (typeID_raw == typeid(float).raw_name())		return PPO_Types::Float;
		if (typeID_raw == typeid(glm::vec2).raw_name()) return PPO_Types::Float2;
		if (typeID_raw == typeid(glm::vec4).raw_name()) return PPO_Types::Float4;
		SHARD3D_FATAL("Invalid type chosen: PPO_helper_getEnumType(const char* typeID_raw)");
	}

	struct RandomShaderParam {
		template <typename T>
		RandomShaderParam(const T& mydata) : dataSizeCPU(sizeof(T)), dataSizeGPU(dataSizeCPU), type(_material_helper_getShaderEnumType(typeid(T).raw_name())) {
			data = malloc(dataSizeCPU);
			memcpy(data, &mydata, dataSizeCPU);
		}
		~RandomShaderParam() {} // figure out how to safely destruct the data pointer, as it's allocated on the heap and thus never gets destroyed
		template <typename T>
		void swap(const T& newParam) { *this = RandomShaderParam(newParam); }
		template <typename T>
		void modify(const T& newVal) { SHARD3D_ASSERT(sizeof(T) == dataSizeCPU); memcpy(data, &newVal, dataSizeCPU); }
		template <typename T>
		T get() { SHARD3D_ASSERT(sizeof(T) == dataSizeCPU); return *reinterpret_cast<T*>(data); }
		void* get() { return data; }
		size_t getGPUSize() { return dataSizeGPU; }
		size_t getCPUSize() { return dataSizeCPU; }
		PPO_Types getType() { return type; }
		RandomShaderParam copy() { return RandomShaderParam(data, dataSizeCPU, type); }
		void free() { if (data) delete data; }
	private:
		RandomShaderParam(const void* mydata, size_t dataSize, PPO_Types type) : dataSizeCPU(dataSize), dataSizeGPU(dataSizeCPU), type(type) {
			data = malloc(dataSizeCPU);
			memcpy(data, mydata, dataSizeCPU);
		}

		size_t dataSizeCPU;
		size_t dataSizeGPU;
		PPO_Types type;
		void* data;
	};

	struct RandomShaderTextureSampler2D {
		RandomShaderTextureSampler2D(const AssetID& _mySampler) : mySampler(_mySampler) {}
		~RandomShaderTextureSampler2D() {}
		AssetID get() { return mySampler; }
		void reload();
	private:
		AssetID mySampler;
	};

	struct _SurfaceMaterialShaderDescriptorInfo {
		VkDescriptorSet factors{};
		VkDescriptorSet textures{};
	};
	
	struct _PPOMaterialShaderDescriptorInfo {
		VkDescriptorSet params{};

		uPtr<EngineDescriptorSetLayout> paramsLayout{};
	};

	struct _HUDMaterialShaderDescriptorInfo {
		VkDescriptorSet data{};

		uPtr<EngineDescriptorSetLayout> dataLayout{};
	};

	enum _SurfaceMaterialBlendMode {
		_SurfaceMaterialBlendModeOpaque = 0,
		_SurfaceMaterialBlendModeMasked = BIT(0),
		_SurfaceMaterialBlendModeTranslucent = BIT(1)
	};

	typedef uint32_t SurfaceMaterialBlendMode_T;

	struct _SurfaceMaterial_BlendModeMasked {
		AssetID maskTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);
	};

	struct _SurfaceMaterial_BlendModeTranslucent {
		AssetID opacityTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);
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

		_SurfaceMaterial_BlendModeMasked* maskedInfo{};
		_SurfaceMaterial_BlendModeTranslucent* translucentInfo{};

		void setBlendMode(SurfaceMaterialBlendMode_T mode);
		SurfaceMaterialBlendMode_T getBlendMode();
		
		void setCullMode(VkCullModeFlags cullmode);
		VkCullModeFlagBits getCullMode();
		void setPolygonMode(VkPolygonMode polygonmode);
		VkPolygonMode getPolygonMode();
	
		SurfaceMaterialClassOptionsFlags getClass() { return classFlags; }
		std::string materialTag = "Some kind of material";

		virtual void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) = 0;
		virtual void serialize(YAML::Emitter* out) = 0;
		virtual void deserialize(YAML::Node* data) = 0;
		virtual void loadAllTextures() = 0;

		void bind(VkCommandBuffer commandBuffer, VkDescriptorSet globalSet);
		inline bool isBuilt() { return built; }
	protected:
		_DrawData drawData{};
		bool built = false;
		uPtr<EngineBuffer> factorsBuffer;
		_SurfaceMaterialShaderDescriptorInfo materialDescriptors;
		SurfaceMaterialClassOptionsFlags classFlags = SurfaceMaterialClassOptions_Opaque | SurfaceMaterialClassOptions_FrontfaceCulling;
		void free();
	private:
	};	

	// SSMR (Specular/Shininess/Metallic Rendering)
	class SurfaceMaterial_Shaded : public SurfaceMaterial {
	public:
		SurfaceMaterial_Shaded() { classFlags |= SurfaceMaterialClassOptions_Shaded; }

		AssetID normalTex = AssetID("assets/_engine/tex/0x807ffe.png" ENGINE_ASSET_SUFFIX);

		glm::vec3 emissiveColor{ 0.f };
		AssetID emissiveTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);

		glm::vec3 diffuseColor{ 1.f };
		AssetID diffuseTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);

		float specular = 0.5f;
		AssetID specularTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);

		float shininess = 0.5f;
		AssetID shininessTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);

		float metallic = 0.f;
		AssetID metallicTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);

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
*	Material that can be used for creating HUD
*	Can be either simple, or complex with custom shaders, accomodated by custom parameters
*/
	class HUDMaterial {
	public:
		HUDMaterial() {}
		~HUDMaterial();
		virtual void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) = 0;

		inline bool isBuilt() { return built; }

		std::string materialTag = "Some kind of material";
	protected:
		bool built = false;
		_HUDMaterialShaderDescriptorInfo materialDescriptorInfo{};
		uPtr<_MaterialGraphicsPipelineConfigInfo> materialPipelineConfig;
	};

	class HUDMaterialBasic : public HUDMaterial {
	public:
		HUDMaterialBasic() {}
		~HUDMaterialBasic();
		void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) override;
		
		glm::vec3 color{ 1.f };
		AssetID colorTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);

		float opacity{ 1.f };
		AssetID opacityTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);

		AssetID maskTex = AssetID("assets/_engine/tex/0xffffff.png" ENGINE_ASSET_SUFFIX);
	};

	class HUDMaterialAdvanced : public HUDMaterial {
	public:
		HUDMaterialAdvanced() {}
		~HUDMaterialAdvanced();
		void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) override;

		template <typename T>
		void addParameter(const T& param, const std::string& param_name) {
			myParamNames.push_back(param_name);
			myParams.emplace_back(param);
		}
		void rmvParameter(uint32_t index);
		RandomShaderParam& getParameter(uint32_t index);
		std::string& getParameterName(uint32_t index);
		template <typename T>
		// Requires rebuilding material if size is different
		void mdfParameter(uint32_t index, const T& newParam, const std::string& newname = "") {
			myParams[index].swap(newParam);
			if (!newname.empty())
				myParamNames[index] = newname;
		}
		size_t getParamCount() { return myParams.size(); }

		std::string shaderPath;
	protected:
		std::vector<std::string> myParamNames;
		std::vector<RandomShaderParam> myParams;

		uPtr<EngineBuffer> myParamsBuffer;
		friend class MaterialManager;
	};

	/*
	*	Material that can be used for post processing effects.
	*	As this is quite complex, you must write shader files for this.
	*/
	class PostProcessingMaterial {
	public:
		PostProcessingMaterial() {}
		~PostProcessingMaterial();
		void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool);
		
		inline bool isBuilt() { return built; }

		template <typename T>
		void addParameter(const T& param, const std::string& param_name) {
			myParamNames.push_back(param_name);
			myParams.emplace_back(param);
		}
		void rmvParameter(uint32_t index);
		RandomShaderParam& getParameter(uint32_t index);
		std::string& getParameterName(uint32_t index);
		template <typename T>
		// Requires rebuilding material if size is different
		void mdfParameter(uint32_t index, const T& newParam, const std::string& newname = "") {
			myParams[index].swap(newParam);
			if (!newname.empty())
				myParamNames[index] = newname;
		}
		size_t getParamCount() { return myParams.size(); }

		std::string materialTag = "Some kind of material";
		std::string shaderPath;
	protected:
 		std::vector<std::string> myParamNames;
		std::vector<RandomShaderParam> myParams;

		bool built = false;
		uPtr<EngineBuffer> myParamsBuffer;
		_PPOMaterialShaderDescriptorInfo materialDescriptorInfo{};
		uPtr<_MaterialComputePipelineConfigInfo> materialPipelineConfig;
		friend class MaterialManager;
		friend class PostProcessingMaterialInstance;
		void free();
	};

	class PostProcessingMaterialInstance {
	public:
		PostProcessingMaterialInstance(PostProcessingMaterial* masterMaterial, AssetID masterAssetID);
		~PostProcessingMaterialInstance();
		void dispatch(VkCommandBuffer commandBuffer, VkDescriptorSet sceneRenderedSet);
		void updateBuffers();

		RandomShaderParam& getParameter(uint32_t index);
		// Do not modify, any modification of this is unsafe
		PostProcessingMaterial* master;
		// just to be able to find what the original asset is
		AssetID masterAsset;
	private:
		std::vector<RandomShaderParam> myParamsLocal;
	};
}