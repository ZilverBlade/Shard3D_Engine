#include "../../s3dpch.h" 

#include "material_system.h"
#include "../../core/asset/assetmgr.h"
#include <fstream>

namespace Shard3D {
	
	namespace MaterialShaderConfigs {
		struct SurfaceMaterial_StandardShadedOpaque {
			VkDescriptorSet normalTex{};
	
			VkDescriptorSet emissiveColor{};
			VkDescriptorSet emissiveTex{};
	
			VkDescriptorSet diffuseColor{};
			VkDescriptorSet diffuseTex{};
	
			VkDescriptorSet specular{};
			VkDescriptorSet specularTex{};
	
			VkDescriptorSet roughness{};
			VkDescriptorSet roughnessTex{};
	
			VkDescriptorSet metallic{};
			VkDescriptorSet metallicTex{};
		};
		struct SurfaceMaterial_StandardShadedMasked {
	
		};
		struct SurfaceMaterial_StandardShadedTranslucent {
	
		};
		struct SurfaceMaterial_StandardShadedMaskedTranslucent {
	
		};
	}
	
	
	MaterialSystem::MaterialSystem(VkRenderPass renderpass, EngineDevice& device, VkDescriptorSetLayout globalSetLayout) : engineDevice { device } {
		SurfaceMaterial_ShadedOpaque::setupMaterialShaderPipeline(device, renderpass, globalSetLayout);

		SurfaceMaterial_ShadedOpaque material{};
		material.createMaterialDescriptors(SharedPools::staticMaterialPool);
	}
	MaterialSystem::~MaterialSystem() {
		//vkDestroyDescriptorSetLayout(engineDevice.device(), materialSetLayout, VK_NULL_HANDLE);
	}
	//
	//void MaterialSystem::modifyCurrentMaterial(SurfaceMaterialData materialData) {
	//	surfaceMaterialData = materialData;
	//	// overwrite
	//}
	//void MaterialSystem::saveList(MaterialList list, const std::string& saveLoc) {
	//	YAML::Emitter out;
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
	//	out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION.toString();
	//	out << YAML::Key << "WBASSET_MaterialList" << YAML::Value << list.materialListTag;
	//	out << YAML::Key << "MaterialListID" << YAML::Value << list.guid;
	//	out << YAML::Key << "List" << YAML::Value << YAML::BeginSeq;
	//
	//	for (Material material : list.list) {
	//		out << YAML::BeginMap;
	//		out << YAML::Key << "Material" << YAML::Value << material.guid;
	//		out << YAML::Key << "Location" << YAML::Value << material.path;
	//		out << YAML::Key << "Pointer" << YAML::Value << "Placeholder";
	//		out << YAML::EndMap;
	//	}
	//
	//	out << YAML::EndSeq;
	//	out << YAML::EndMap;
	//
	//	auto newPath = saveLoc;
	//	if (!strUtils::hasEnding(saveLoc, ".wbasset")) newPath = saveLoc + ".wbasset";
	//	std::ofstream fout(newPath);
	//	fout << out.c_str();
	//	fout.flush();
	//	fout.close();
	//}
	//
	//void MaterialSystem::saveMaterial(Material materialData, const std::string & saveLoc) {
	//	YAML::Emitter out;
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
	//	out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION.toString();
	//	out << YAML::Key << "WBASSET_Material" << YAML::Value << materialData.materialTag;
	//	out << YAML::Key << "MaterialID" << YAML::Value << materialData.guid;
	//	out << YAML::Key << "Material" << YAML::Value << YAML::BeginSeq;
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Container" << YAML::Value;
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Physics";
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "MaterialStruct" << YAML::Value << "SurfaceMaterial";
	//	out << YAML::Key << "MaterialType" << YAML::Value << stringFromEnum(materialData.surfaceMaterial.surfaceMat);
	//	out << YAML::Key << "MaterialProperties" << YAML::Value << stringFromEnum(materialData.surfaceMaterial.surfaceProp);
	//	out << YAML::EndMap;
	//
	//	out << YAML::Key << "Draw";
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "culling" << YAML::Value << materialData.surfaceMaterial.drawData.culling;
	//	out << YAML::Key << "polygonMode" << YAML::Value << materialData.surfaceMaterial.drawData.polygonMode;
	//	out << YAML::EndMap;
	//
	//	out << YAML::Key << "Data";
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "normalTex" << YAML::Value << materialData.surfaceMaterial.normalTex.path;
	//	out << YAML::Key << "diffuseVec4" << YAML::Value << materialData.surfaceMaterial.diffuseColor;
	//	out << YAML::Key << "diffuseTex" << YAML::Value << materialData.surfaceMaterial.diffuseTex.path;
	//	out << YAML::Key << "specularFloat" << YAML::Value << materialData.surfaceMaterial.specular;
	//	out << YAML::Key << "specularTex" << YAML::Value << materialData.surfaceMaterial.specularTex.path;
	//	out << YAML::Key << "roughnessFloat" << YAML::Value << materialData.surfaceMaterial.roughness;
	//	out << YAML::Key << "roughnessTex" << YAML::Value << materialData.surfaceMaterial.roughnessTex.path;
	//	out << YAML::Key << "metallicFloat" << YAML::Value << materialData.surfaceMaterial.metallic;
	//	out << YAML::Key << "metallicTex" << YAML::Value << materialData.surfaceMaterial.metallicTex.path;
	//
	//	if (materialData.surfaceMaterial.surfaceMat == SurfaceMaskedMaterial || materialData.surfaceMaterial.surfaceMat == SurfaceMaskedTranslucentMaterial)
	//		out << YAML::Key << "maskTex" << YAML::Value << materialData.surfaceMaterial.maskTex.path;
	//	out << YAML::EndMap;
	//	out << YAML::EndSeq;
	//	out << YAML::EndMap;
	//	out << YAML::EndMap;
	//
	//	auto newPath = saveLoc;
	//	if (!strUtils::hasEnding(saveLoc, ".wbasset")) newPath = saveLoc + ".wbasset";
	//	std::ofstream fout(newPath);
	//	fout << out.c_str();
	//	fout.flush();
	//	fout.close();
	//
	//	// this is strictly a local path, which means all materials must be in this folder
	//	materialData.path = newPath;//.substr(newPath.rfind("assets\\materialdata"));
	//	AssetManager::emplaceMaterial(materialData);
	//}
	//
	//Material MaterialSystem::loadMaterial(const std::string& loadLoc, bool ignoreWarns) {
	//
	//}
	//MaterialSystem::MaterialList MaterialSystem::loadList(const std::string& loadLoc, bool ignoreWarns) {
	//	std::ifstream stream(loadLoc);
	//	std::stringstream strStream;
	//	strStream << stream.rdbuf();
	//
	//	YAML::Node data = YAML::Load(strStream.str());
	//
	//	if (!data["WBASSET_MaterialList"]) return MaterialList();
	//
	//
	//	if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
	//		SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect engine version");
	//	}// change this to check if the version is less or more
	//	if (data["WorldBuilder3D"].as<std::string>() != EDITOR_VERSION.toString()) {
	//		SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect editor version");
	//	}// change this to check if the version is less or more
	//
	//	std::string levelName = data["Level"].as<std::string>();
	//
	//	if (data["List"]) {
	//		MaterialList loadedList{};
	//
	//		SHARD3D_INFO("Loading material list with ID {0}", data["MaterialListID"].as<uint64_t>());
	//
	//		loadedList.materialListTag = data["WBASSET_MaterialList"].as<std::string>();
	//		loadedList.guid = data["MaterialListID"].as<uint64_t>();
	//
	//		for (auto material : data["Material"]) {
	//			loadedList.list.push_back(loadMaterial(material["Location"].as<std::string>(), true));
	//		}
	//		return loadedList;
	//	}
	//}
	void MaterialSystem::createDescriptorSetLayout() {
		
	}

}
