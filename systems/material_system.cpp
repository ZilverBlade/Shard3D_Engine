#include "material_system.hpp"
#include "../utils/engine_utils.hpp"

namespace Shard3D {
	MaterialSystem::MaterialSystem(SurfaceMaterialData materialData, EngineDevice& device, VkDescriptorSetLayout matSetLayout) : surfaceMaterialData{materialData}, engineDevice { device }, materialSetLayout(matSetLayout) {
		//createDescriptorSetLayout();
	}
	MaterialSystem::~MaterialSystem() {
		//vkDestroyDescriptorSetLayout(engineDevice.device(), materialSetLayout, VK_NULL_HANDLE);
	}

	void MaterialSystem::modifyCurrentMaterial(SurfaceMaterialData materialData) {
		surfaceMaterialData = materialData;
		// overwrite
	}

	void MaterialSystem::saveMaterial(SurfaceMaterialData materialData, const std::string& saveLoc) {
		json out = "{";
		out.emplace_back("\""+ materialData.atMeshName + "\" { ");
		out.emplace_back("\"MaterialStruct\": \"SurfaceMaterial\",");
		out.emplace_back("\"MaterialType\": \"" + stringFromEnum(materialData.surfaceMat) + "\",");
		out.emplace_back("\"MaterialProperties\": \"" + stringFromEnum(materialData.surfaceProp) + "\",");
		out.emplace_back("\"Data\": [");
		out.emplace_back("{");
			
		out.emplace_back("\"normalTex\": " + materialData.normalTex.path + ",");
		out.emplace_back("\"diffuseVec4\": " + stringFromVec4(materialData.diffuseColor) + ",");
		out.emplace_back("\"diffuseTex\": " + materialData.diffuseTex.path + ",");
		out.emplace_back("\"specularFloat\": " + std::to_string(materialData.specular) + ",");
		out.emplace_back("\"specularTex\": " + materialData.specularTex.path + ",");
		out.emplace_back("\"roughnessFloat\": " + std::to_string(materialData.roughness) + ",");
		out.emplace_back("\"roughnessTex\": " + materialData.roughnessTex.path + ",");
		out.emplace_back("\"metallicFloat\": " + std::to_string(materialData.metallic) + ",");
		out.emplace_back("\"metallicTex\": " + materialData.metallicTex.path + ",");

		if (materialData.surfaceMat == MaskedMaterial || materialData.surfaceMat == MaskedTranslucentMaterial)
			out.emplace_back("\"maskTex\": " + materialData.maskTex.path + ",");
		out.emplace_back("\"null\": 0");
		out.emplace_back("},");
		out.emplace_back("\"culling\": none");
		out.emplace_back("]");
		out.emplace_back("}");
		out.emplace_back("}");

		auto newPath = saveLoc;
		if (!strUtils::hasEnding(saveLoc, ".staticmat.json")) newPath = saveLoc + ".staticmat.json";
		std::ofstream fout(newPath);
		fout << out.dump();
		fout.flush();
		fout.close();
	}

	void MaterialSystem::createDescriptorSetLayout() {
		SharedPools::staticMaterialPool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
	}
}