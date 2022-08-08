#pragma once


#include "../utils/definitions.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../descriptor_pools.hpp"
#include "../renderer.hpp"
#include "../GUID.hpp"

namespace Shard3D {
	enum MaterialType {
		NullMaterial = 0,

		// Material that can be used on meshes, to be rendered visually. 
		// Can have PBR, texture maps, and be transparent.
		SurfaceMaterial = 1,

		// decal and post process arent guaranteed to make it, however they are here to future proof

		// Material that can be used for projecting on meshes, to be rendered visually. 
		// It will overlay whatever previous material with it's properties and render over it.
		DecalMaterial = 4,
		// Material that can be used for post processing effects.
		// As this is quite complex, it will exclusively to a shader file, where you can do your magic.
		PostProcessMaterial = 5
	};
	enum SurfaceMaterialType {
		SurfaceNullMaterial = 0,

		SurfaceOpaqueMaterial = 1,
		SurfaceMaskedMaterial = 2,
		SurfaceTranslucentMaterial = 3,
		SurfaceMaskedTranslucentMaterial = 4
	};
	enum SurfaceMaterialProperties {
		SurfaceNullMaterialProperties = 0,

		SurfaceStandardLit = 1,
		SurfaceStandardUnlit = 2,

		SurfaceClearcoat = 3,
	};
	class MaterialSystem {
#pragma region Protected
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
		struct SurfaceMaterialData {
			SurfaceMaterialType surfaceMat = SurfaceOpaqueMaterial;
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
			std::string shader;
		};
#pragma endregion
	public:	
		struct Material {
			MaterialType type;
			SurfaceMaterialData surfaceMaterial;
			DecalMaterialData decalMaterial;
			PostProcessMaterialData postProcessMaterial;

			// not relevant for the shaders since this will be handled in the material system
			std::string materialTag = "Some kind of material";
			std::string path; // mostly for saving, as this is necessary for the material list finder to know where to look
							  // this is also to avoid needing to load and store all of the materials at first and then clear unused ones,
							  // speeding up loading times
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
		static void saveList(MaterialList list, const std::string& saveLoc);
		static void saveMaterial(Material materialData, const std::string& saveLoc);
		static Material loadMaterial(const std::string& loadLoc, bool ignoreWarns = true);
		static MaterialList loadList(const std::string& loadLoc, bool ignoreWarns = true);
	private:
		void createDescriptorSetLayout();

		EngineDevice& engineDevice;
		SurfaceMaterialData surfaceMaterialData;
		VkDescriptorSetLayout materialSetLayout;
#pragma region Helper stuff
		static inline std::string stringFromEnum(MaterialType enm) {
			if (enm == SurfaceMaterial)				return "SurfaceMaterial";
			if (enm == DecalMaterial)				return "DecalMaterial";
			if (enm == PostProcessMaterial)			return "PostProcessMaterial";
			return "ERROR_MAT";
		}
		static inline std::string stringFromEnum(SurfaceMaterialProperties enm) {
			if (enm == SurfaceStandardLit)			return "SurfaceStandardLit";
			if (enm == SurfaceStandardUnlit)		return "SurfaceStandardUnlit";
			if (enm == SurfaceClearcoat)			return "SurfaceClearcoat";
			return "Null";
		}
		static inline std::string stringFromEnum(SurfaceMaterialType enm) {
			if (enm == SurfaceOpaqueMaterial)				return "OpaqueMaterial";
			if (enm == SurfaceMaskedMaterial)				return "MaskedMaterial";
			if (enm == SurfaceTranslucentMaterial)			return "TranslucentMaterial";
			if (enm == SurfaceMaskedTranslucentMaterial)	return "MaskedTranslucentMaterial";
			return "Null";
		}
		static inline int enumFromString(const std::string& input) {
			if (input == "SurfaceMaterial")			return SurfaceMaterial;
			if (input == "DecalMaterial")			return DecalMaterial;
			if (input == "PostProcessMaterial")		return PostProcessMaterial;

			if (input == "SurfaceStandardLit")			return SurfaceStandardLit;
			if (input == "SurfaceStandardUnlit")		return SurfaceStandardUnlit;
			if (input == "SurfaceClearcoat")			return SurfaceClearcoat;

			if (input == "OpaqueMaterial")				return SurfaceOpaqueMaterial;
			if (input == "MaskedMaterial")				return SurfaceMaskedMaterial;
			if (input == "TranslucentMaterial")			return SurfaceTranslucentMaterial;
			if (input == "MaskedTranslucentMaterial")	return SurfaceMaskedTranslucentMaterial;
		}
#pragma endregion
	};
}