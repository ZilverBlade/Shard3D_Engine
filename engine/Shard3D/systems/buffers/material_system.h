#pragma once

#include "../../core.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/UUID.h"
#include "../../core/asset/material.h"

namespace Shard3D {
	class MaterialSystem {
	public:
		MaterialSystem(VkRenderPass renderpass, EngineDevice& device, VkDescriptorSetLayout globalSetLayout);

		~MaterialSystem();

		//VkDescriptorSet* createMaterialDescriptor(Material* material, EngineDescriptorSetLayout* factorsSetLayout, EngineDescriptorSetLayout* imagesSetLayout, EngineDescriptorPool& descriptorPool);
		//void createDescriptorPools();
		//
		//void modifyCurrentMaterial(SurfaceMaterialData materialData);
		//static void saveList(MaterialList list, const std::string& saveLoc);
		//static void saveMaterial(Material materialData, const std::string& saveLoc);
		//static Material loadMaterial(const std::string& loadLoc, bool ignoreWarns = true);
		//static MaterialList loadList(const std::string& loadLoc, bool ignoreWarns = true);
	private:
		void createDescriptorSetLayout();

		EngineDevice& engineDevice;
		//SurfaceMaterialData surfaceMaterialData;
		VkDescriptorSetLayout materialSetLayout;
#pragma region Helper stuff
		//static inline std::string stringFromEnum(MaterialType enm) {
		//	if (enm == SurfaceMaterial)				return "SurfaceMaterial";
		//	if (enm == DecalMaterial)				return "DecalMaterial";
		//	if (enm == PostProcessMaterial)			return "PostProcessMaterial";
		//	return "ERROR_MAT";
		//}
		//static inline std::string stringFromEnum(SurfaceMaterialProperties enm) {
		//	if (enm == SurfaceStandardLit)			return "SurfaceStandardLit";
		//	if (enm == SurfaceStandardUnlit)		return "SurfaceStandardUnlit";
		//	if (enm == SurfaceClearcoat)			return "SurfaceClearcoat";
		//	return "Null";
		//}
		//static inline std::string stringFromEnum(SurfaceMaterialType enm) {
		//	if (enm == SurfaceOpaqueMaterial)				return "OpaqueMaterial";
		//	if (enm == SurfaceMaskedMaterial)				return "MaskedMaterial";
		//	if (enm == SurfaceTranslucentMaterial)			return "TranslucentMaterial";
		//	if (enm == urfaceMaskedTranslucentMaterial)	return "MaskedTranslucentMaterial";
		//	return "Null";
		//}
		//static inline int enumFromString(const std::string& input) {
		//	if (input == "SurfaceMaterial")			return SurfaceMaterial;
		//	if (input == "DecalMaterial")			return DecalMaterial;
		//	if (input == "PostProcessMaterial")		return PostProcessMaterial;
		//
		//	if (input == "SurfaceStandardLit")			return SurfaceStandardLit;
		//	if (input == "SurfaceStandardUnlit")		return SurfaceStandardUnlit;
		//	if (input == "SurfaceClearcoat")			return SurfaceClearcoat;
		//
		//	if (input == "OpaqueMaterial")				return SurfaceOpaqueMaterial;
		//	if (input == "MaskedMaterial")				return SurfaceMaskedMaterial;
		//	if (input == "TranslucentMaterial")			return SurfaceTranslucentMaterial;
		//	if (input == "MaskedTranslucentMaterial")	return SurfaceMaskedTranslucentMaterial;
		//}
#pragma endregion
	};
	
}
