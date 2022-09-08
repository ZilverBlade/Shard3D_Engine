#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"
#include "../asset/assetid.h"
#include "../vulkan_api/pipeline_compute.h"
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
	struct _MaterialGraphicsPipelineConfigInfo {
		VkPipelineLayout shaderPipelineLayout{};
		uPtr<GraphicsPipeline> shaderPipeline{};
	};
	struct _MaterialComputePipelineConfigInfo {
		VkPipelineLayout shaderPipelineLayout{};
		uPtr<ComputePipeline> shaderPipeline{};
	};
	struct _DrawData {
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlags culling = VK_CULL_MODE_NONE;
	};

	struct _SurfaceMaterialShaderDescriptorInfo {
		VkDescriptorSet factors{};
		VkDescriptorSet textures{};

		uPtr<EngineDescriptorSetLayout> factorLayout{};
		uPtr<EngineDescriptorSetLayout> textureLayout{};
	};

	struct _PPOMaterialShaderDescriptorInfo {
		VkDescriptorSet params{};

		uPtr<EngineDescriptorSetLayout> paramsLayout{};
	};

	enum SurfaceMaterialBlendMode {
		SurfaceMaterialBlendModeOpaque = 0,
		SurfaceMaterialBlendModeMasked = BIT(0),
		SurfaceMaterialBlendModeTranslucent = BIT(1)
	};

#define SurfaceMaterialBlendMode_T uint32_t

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

		uint32_t getBlendMode() { return blendMode; }
		void setBlendMode(SurfaceMaterialBlendMode_T mode);

		std::string materialTag = "Some kind of material";

		_DrawData drawData{};

		virtual void createMaterialShader(EngineDevice& device, uPtr<EngineDescriptorPool>& descriptorPool) = 0;
		virtual void serialize(YAML::Emitter* out) = 0;
		virtual void deserialize(YAML::Node* data) = 0;
		virtual void loadAllTextures() = 0;

		void bind(VkCommandBuffer commandBuffer, VkDescriptorSet globalSet);
		inline bool isBuilt() { return built; }
		VkPipelineLayout getPipelineLayout() { return materialPipelineConfig->shaderPipelineLayout; }
	protected:
		bool built = false;
		uPtr<_MaterialGraphicsPipelineConfigInfo> materialPipelineConfig{};
		_SurfaceMaterialShaderDescriptorInfo materialDescriptorInfo{};
		uPtr<EngineBuffer> factorsBuffer;
		SurfaceMaterialBlendMode_T blendMode = SurfaceMaterialBlendModeOpaque;
	};	

	// SSMR (Specular/Shininess/Metallic Rendering)
	class SurfaceMaterial_Shaded : public SurfaceMaterial {
	public:
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
	*	Material that can be used for post processing effects.
	*	As this is quite complex, you must write shader files for this.
	*/

	enum class PPO_Types {
		Int8 = 0,
		Int16 = 1,
		Int32 = 2,
		Float = 3,
		Float2 = 4,
		Float3 = 5,
		Float4 = 6
	};
		
	// Helper function: 
	// Gets the enum type from a raw typeid name
	static PPO_Types _PPO_helper_getEnumType(const char* typeID_raw) {
		if (typeID_raw == typeid(char).raw_name())		return PPO_Types::Int8;
		if (typeID_raw == typeid(short).raw_name())		return PPO_Types::Int16;
		if (typeID_raw == typeid(int).raw_name())		return PPO_Types::Int32;
		if (typeID_raw == typeid(float).raw_name())		return PPO_Types::Float;
		if (typeID_raw == typeid(glm::vec2).raw_name()) return PPO_Types::Float2;
		if (typeID_raw == typeid(glm::vec3).raw_name()) return PPO_Types::Float3;
		if (typeID_raw == typeid(glm::vec4).raw_name()) return PPO_Types::Float4;
		SHARD3D_FATAL("Invalid type chosen: PPO_helper_getEnumType(const char* typeID_raw)");
	}

	struct RandomPPOParam {
		template <typename T>
		RandomPPOParam(const T& mydata) : dataSizeCPU(sizeof(T)), dataSizeGPU(dataSizeCPU), type(_PPO_helper_getEnumType(typeid(T).raw_name())) {
			SHARD3D_ASSERT(type != PPO_Types::Float3 && "pain was used");
			data = malloc(dataSizeCPU);
			memcpy(data, &mydata, dataSizeCPU);
		}
		~RandomPPOParam() {} // figure out how to safely destruct the data pointer, as it's allocated on the heap and thus never gets destroyed
		template <typename T>
		void swap(const T& newParam) { *this = RandomPPOParam(newParam); }
		template <typename T>
		void modify(const T& newData) { SHARD3D_ASSERT(sizeof(T) == dataSizeCPU); memcpy(data, &newData, dataSizeCPU); }
		template <typename T>
		T get() { SHARD3D_ASSERT(sizeof(T) == dataSizeCPU); return *reinterpret_cast<T*>(data); }
		void* get() { return data; }
		size_t getGPUSize() { return dataSizeGPU; }
		size_t getCPUSize() { return dataSizeCPU; }
		PPO_Types getType() { return type; }
		RandomPPOParam copy() { return RandomPPOParam(data, dataSizeCPU, type); }
		void free() { if (data) delete data; }
	private:
		RandomPPOParam(const void* mydata, size_t dataSize, PPO_Types type) : dataSizeCPU(dataSize), dataSizeGPU(dataSizeCPU), type(type) {
			SHARD3D_ASSERT(type != PPO_Types::Float3 && "pain was used");
			data = malloc(dataSizeCPU);
			memcpy(data, mydata, dataSizeCPU);
		}

		size_t dataSizeCPU;
		size_t dataSizeGPU;
		PPO_Types type;
		void* data;
	};

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
		RandomPPOParam& getParameter(uint32_t index);
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
		std::vector<RandomPPOParam> myParams;

		bool built = false;
		uPtr<EngineBuffer> myParamsBuffer;
		_PPOMaterialShaderDescriptorInfo materialDescriptorInfo{};
		uPtr<_MaterialComputePipelineConfigInfo> materialPipelineConfig;
		friend class MaterialManager;
		friend class PostProcessingMaterialInstance;
	};

	class PostProcessingMaterialInstance {
	public:
		PostProcessingMaterialInstance(PostProcessingMaterial* masterMaterial);
		~PostProcessingMaterialInstance();
		void dispatch(VkCommandBuffer commandBuffer, VkDescriptorSet sceneRenderedSet);
		void updateBuffers();

		RandomPPOParam& getParameter(uint32_t index);
		// Do not modify, any modification of this is unsafe
		PostProcessingMaterial* master;
	private:
		std::vector<RandomPPOParam> myParamsLocal;
	};
}