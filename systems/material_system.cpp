#include "material_system.hpp"
#include "../utils/engine_utils.hpp"
#include "../utils/yaml_ext.hpp"

namespace Shard3D {
	MaterialSystem::MaterialSystem(SurfaceMaterialData materialData, EngineDevice& device, VkDescriptorSetLayout matSetLayout) : surfaceMaterialData{materialData}, engineDevice { device }, materialSetLayout(matSetLayout) {
		//createDescriptorSetLayout();
	}
	MaterialSystem::~MaterialSystem() {
		//vkDestroyDescriptorSetLayout(engineDevice.device(), materialSetLayout, VK_NULL_HANDLE);
	}

	void MaterialSystem::createDescriptorPools() {
		SharedPools::staticMaterialPool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		auto materialSetLayout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();
	}

	void MaterialSystem::modifyCurrentMaterial(SurfaceMaterialData materialData) {
		surfaceMaterialData = materialData;
		// overwrite
	}

	void MaterialSystem::saveMaterial(SurfaceMaterialData materialData, const std::string& saveLoc) {
		YAML::Emitter out;
		out << YAML::BeginMap;
			out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION;
			out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION;
			out << YAML::Key << "WBASSET_Material" << YAML::Value << materialData.materialTag;
			out << YAML::Key << "MaterialID" << YAML::Value << materialData.guid;
			out << YAML::Key << "Material" << YAML::Value << YAML::BeginSeq;
			out << YAML::BeginMap;
			out << YAML::Key << "Container" << YAML::Value;
			out << YAML::BeginMap;
				out << YAML::Key << "Physics";
				out << YAML::BeginMap;
					out << YAML::Key << "MaterialStruct" << YAML::Value << "SurfaceMaterial";
					out << YAML::Key << "MaterialType" << YAML::Value << stringFromEnum(materialData.surfaceMat);
					out << YAML::Key << "MaterialProperties" << YAML::Value << stringFromEnum(materialData.surfaceProp);
				out << YAML::EndMap;

				out << YAML::Key << "Draw";
				out << YAML::BeginMap;
					out << YAML::Key << "culling" << YAML::Value << materialData.drawData.culling;
					out << YAML::Key << "polygonMode" << YAML::Value << materialData.drawData.polygonMode;
				out << YAML::EndMap;

				out << YAML::Key << "Data";
				out << YAML::BeginMap;
					out << YAML::Key << "normalTex" << YAML::Value << materialData.normalTex.path;
					out << YAML::Key << "diffuseVec4" << YAML::Value << materialData.diffuseColor;
					out << YAML::Key << "diffuseTex" << YAML::Value << materialData.diffuseTex.path;
					out << YAML::Key << "specularFloat" << YAML::Value << materialData.specular;
					out << YAML::Key << "specularTex" << YAML::Value << materialData.specularTex.path;
					out << YAML::Key << "roughnessFloat" << YAML::Value << materialData.roughness;
					out << YAML::Key << "roughnessTex" << YAML::Value << materialData.roughnessTex.path;
					out << YAML::Key << "metallicFloat" << YAML::Value <<  materialData.metallic;
					out << YAML::Key << "metallicTex" << YAML::Value << materialData.metallicTex.path;

			if (materialData.surfaceMat == MaskedMaterial || materialData.surfaceMat == MaskedTranslucentMaterial)
					out << YAML::Key << "maskTex" << YAML::Value << materialData.maskTex.path;
				out << YAML::EndMap;
		out << YAML::EndSeq;
		out << YAML::EndMap;
		out << YAML::EndMap;

		auto newPath = saveLoc;
		if (!strUtils::hasEnding(saveLoc, ".wbasset")) newPath = saveLoc + ".wbasset";
		std::ofstream fout(newPath);
		fout << out.c_str();
		fout.flush();
		fout.close();
	}

	void MaterialSystem::createDescriptorSetLayout() {
		
	}
}