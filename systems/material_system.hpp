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
			VkImage image;
			VkImageView imageView;
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
	protected:
		struct SurfaceMaterialData {
			SurfaceMaterialType surfaceMat = OpaqueMaterial;
			SurfaceMaterialProperties surfaceProp = SurfaceStandardLit;
			DrawData drawData;

			MaterialTexture normalTex = { "assets/texturedata/0x807ffe.png", VkSampler() };

			//glm::vec4 emissiveColor{ 0.f };
			//MaterialTexture emissiveTex = { "0xffffff.png", VkSampler() };

			glm::vec4 diffuseColor{ 1.f };
			MaterialTexture diffuseTex = { "assets/texturedata/0xffffff.png", VkSampler() };

			float specular = 0.5f;
			MaterialTexture specularTex = { "assets/texturedata/0xffffff.png", VkSampler() };

			float roughness = 0.5f;
			MaterialTexture roughnessTex = { "assets/texturedata/0xffffff.png", VkSampler() };

			float metallic = 0.f;
			MaterialTexture metallicTex = { "assets/texturedata/0xffffff.png", VkSampler() };

			MaterialTexture maskTex = { "assets/texturedata/0xffffff.png", VkSampler() };
		};
		struct DecalMaterialData {
			int nothing; // literally dfk what do do as placeholder lol
		};
		struct PostProcessMaterialData {
			VkSampler viewportOut;
		};
	public:
		
		struct Material {
			MaterialType type;
			SurfaceMaterialData surfaceMaterial;
			DecalMaterialData decalMaterial;
			PostProcessMaterialData postProcessMaterial;

			// not relevant for the shaders since this will be handled in the material system
			std::string materialTag = "Some kind of material";
			GUID guid;
		};

		struct MaterialList {
			std::vector<Material> list;
			std::string materialListTag = "Some kind of material list";
			GUID guid;
		};

		MaterialSystem(SurfaceMaterialData materialData, 
			EngineDevice& device, 
			VkDescriptorSetLayout materialSetLayout
		);

		~MaterialSystem();

		void createDescriptorPools();

		void modifyCurrentMaterial(SurfaceMaterialData materialData);
		static void saveMaterial(Material materialData, const std::string& saveLoc);
		static Material loadMaterial(const std::string& loadLoc, bool ignoreWarns = true);
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

		inline static int enumFromString(const std::string& input) {
			if (input == "SurfaceMaterial")			return SurfaceMaterial;
			if (input == "DecalMaterial")			return DecalMaterial;
			if (input == "PostProcessMaterial")		return PostProcessMaterial;

			if (input == "SurfaceStandardLit")			return SurfaceStandardLit;
			if (input == "SurfaceStandardUnlit")		return SurfaceStandardUnlit;
			if (input == "SurfaceClearcoat")			return SurfaceClearcoat;

			if (input == "OpaqueMaterial")				return OpaqueMaterial;
			if (input == "MaskedMaterial")				return MaskedMaterial;
			if (input == "TranslucentMaterial")			return TranslucentMaterial;
			if (input == "MaskedTranslucentMaterial")	return MaskedTranslucentMaterial;
		}
	};
}