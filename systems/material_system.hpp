#pragma once
#include <glm/glm.hpp>
#include <string>

#include "../utils/definitions.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../descriptor_pools.hpp"
#include "../renderer.hpp"
#include "../GUID.hpp"

namespace Shard3D {
	enum MaterialType {
		NullMaterial = 0,

		SurfaceMaterial= 1,	// unlit will get passed to a lower cost shader

		DecalMaterial = 4,
		PostProcessMaterial = 5	// decal and post process arent guaranteed to make it, however they are here to future proof
	};
	enum SurfaceMaterialType {
		SurfaceNullMaterial = 0,

		OpaqueMaterial = 1,
		MaskedMaterial = 2,
		TranslucentMaterial = 3,
		MaskedTranslucentMaterial = 4
	};
	enum SurfaceMaterialProperties {
		SurfaceNullMaterialProperties = 0,

		SurfaceStandardLit = 1,
		SurfaceStandardUnlit = 2,

		SurfaceClearcoat = 3,
	};
	class MaterialSystem {
	protected:
		class MaterialTexture {
		public:
			MaterialTexture() = default;
			MaterialTexture(const std::string& p, VkSampler s) : path(p), sampler(s) {}

			std::string path;
			VkSampler sampler;
		};

		enum Culling {
			NoCulling = 0,
			FrontCulling = 1,
			BackCulling = 2,
			BothCulling = 3,
		};
		struct DrawData {
			VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
			VkCullModeFlags culling = VK_CULL_MODE_NONE;
		};
	public:
		struct SurfaceMaterialData {
			SurfaceMaterialType surfaceMat = OpaqueMaterial;
			SurfaceMaterialProperties surfaceProp = SurfaceStandardLit;
			DrawData drawData;

			MaterialTexture normalTex = { "0x807ffe.png", VkSampler() };

			//glm::vec4 emissiveColor{ 0.f };
			//MaterialTexture emissiveTex = { "0xffffff.png", VkSampler() };

			glm::vec4 diffuseColor{1.f};
			MaterialTexture diffuseTex = { "0xffffff.png", VkSampler() };
		
			float specular = 0.5f;
			MaterialTexture specularTex = { "0xffffff.png", VkSampler() };

			float roughness = 0.5f;
			MaterialTexture roughnessTex = { "0xffffff.png", VkSampler() };

			float metallic = 0.f;
			MaterialTexture metallicTex = { "0xffffff.png", VkSampler() };

			MaterialTexture maskTex = { "0xffffff.png", VkSampler() };

			// not relevant for the shaders since this will be handled in the material system
			std::string materialTag = "Some kind of material";
			GUID guid;
		};

		struct PostProcessMaterialData {
			VkSampler viewportOut;
		};

		MaterialSystem(SurfaceMaterialData materialData, 
			EngineDevice& device, 
			VkDescriptorSetLayout materialSetLayout
		);

		~MaterialSystem();

		void createDescriptorPools();

		void modifyCurrentMaterial(SurfaceMaterialData materialData);
		static void saveMaterial(SurfaceMaterialData materialData, const std::string& saveLoc);
	private:
		void createDescriptorSetLayout();

		EngineDevice& engineDevice;
		SurfaceMaterialData surfaceMaterialData;
		VkDescriptorSetLayout materialSetLayout;

		inline static std::string stringFromEnum(MaterialType enm) {
			if (enm == SurfaceMaterial)				return "SurfaceMaterial";
			if (enm == DecalMaterial)				return "DecalMaterial";
			if (enm == PostProcessMaterial)			return "PostProcessMaterial";
			return "ERROR_MAT";
		}
		inline static std::string stringFromEnum(SurfaceMaterialProperties enm) {
			if (enm == SurfaceStandardLit)			return "SurfaceStandardLit";
			if (enm == SurfaceStandardUnlit)		return "SurfaceStandardUnlit";
			if (enm == SurfaceClearcoat)			return "SurfaceClearcoat";
			return "Null";
		}
		inline static std::string stringFromEnum(SurfaceMaterialType enm) {
			if (enm == OpaqueMaterial)				return "OpaqueMaterial";
			if (enm == MaskedMaterial)				return "MaskedMaterial";
			if (enm == TranslucentMaterial)			return "TranslucentMaterial";
			if (enm == MaskedTranslucentMaterial)	return "MaskedTranslucentMaterial";
			return "Null";
		}

		inline static std::string stringFromVec4(glm::vec4 vec4) {
			std::string float4 = "[ " +
				std::to_string(vec4.x) + ", " +
				std::to_string(vec4.y) + ", " +
				std::to_string(vec4.z) + ", " +
				std::to_string(vec4.w) + " ]";
			return float4;
		}
	};
}