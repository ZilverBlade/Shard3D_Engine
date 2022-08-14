#include "../../s3dpch.h" 

#include "material_system.h"
#include "../../core/asset/assetmgr.h"
#include <fstream>
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
			.addBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();
	}

	void MaterialSystem::modifyCurrentMaterial(SurfaceMaterialData materialData) {
		surfaceMaterialData = materialData;
		// overwrite
	}
	void MaterialSystem::saveList(MaterialList list, const std::string& saveLoc) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
		out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION.toString();
		out << YAML::Key << "WBASSET_MaterialList" << YAML::Value << list.materialListTag;
		out << YAML::Key << "MaterialListID" << YAML::Value << list.guid;
		out << YAML::Key << "List" << YAML::Value << YAML::BeginSeq;

		for (Material material : list.list) {
			out << YAML::BeginMap;
			out << YAML::Key << "Material" << YAML::Value << material.guid;
			out << YAML::Key << "Location" << YAML::Value << material.path;
			out << YAML::Key << "Pointer" << YAML::Value << "Placeholder";
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		auto newPath = saveLoc;
		if (!strUtils::hasEnding(saveLoc, ".wbasset")) newPath = saveLoc + ".wbasset";
		std::ofstream fout(newPath);
		fout << out.c_str();
		fout.flush();
		fout.close();
	}

	void MaterialSystem::saveMaterial(Material materialData, const std::string & saveLoc) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
		out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION.toString();
		out << YAML::Key << "WBASSET_Material" << YAML::Value << materialData.materialTag;
		out << YAML::Key << "MaterialID" << YAML::Value << materialData.guid;
		out << YAML::Key << "Material" << YAML::Value << YAML::BeginSeq;
		out << YAML::BeginMap;
		out << YAML::Key << "Container" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Physics";
		out << YAML::BeginMap;
		out << YAML::Key << "MaterialStruct" << YAML::Value << "SurfaceMaterial";
		out << YAML::Key << "MaterialType" << YAML::Value << stringFromEnum(materialData.surfaceMaterial.surfaceMat);
		out << YAML::Key << "MaterialProperties" << YAML::Value << stringFromEnum(materialData.surfaceMaterial.surfaceProp);
		out << YAML::EndMap;

		out << YAML::Key << "Draw";
		out << YAML::BeginMap;
		out << YAML::Key << "culling" << YAML::Value << materialData.surfaceMaterial.drawData.culling;
		out << YAML::Key << "polygonMode" << YAML::Value << materialData.surfaceMaterial.drawData.polygonMode;
		out << YAML::EndMap;

		out << YAML::Key << "Data";
		out << YAML::BeginMap;
		out << YAML::Key << "normalTex" << YAML::Value << materialData.surfaceMaterial.normalTex.path;
		out << YAML::Key << "diffuseVec4" << YAML::Value << materialData.surfaceMaterial.diffuseColor;
		out << YAML::Key << "diffuseTex" << YAML::Value << materialData.surfaceMaterial.diffuseTex.path;
		out << YAML::Key << "specularFloat" << YAML::Value << materialData.surfaceMaterial.specular;
		out << YAML::Key << "specularTex" << YAML::Value << materialData.surfaceMaterial.specularTex.path;
		out << YAML::Key << "roughnessFloat" << YAML::Value << materialData.surfaceMaterial.roughness;
		out << YAML::Key << "roughnessTex" << YAML::Value << materialData.surfaceMaterial.roughnessTex.path;
		out << YAML::Key << "metallicFloat" << YAML::Value << materialData.surfaceMaterial.metallic;
		out << YAML::Key << "metallicTex" << YAML::Value << materialData.surfaceMaterial.metallicTex.path;

		if (materialData.surfaceMaterial.surfaceMat == SurfaceMaskedMaterial || materialData.surfaceMaterial.surfaceMat == SurfaceMaskedTranslucentMaterial)
			out << YAML::Key << "maskTex" << YAML::Value << materialData.surfaceMaterial.maskTex.path;
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

		// this is strictly a local path, which means all materials must be in this folder
		materialData.path = newPath;//.substr(newPath.rfind("assets\\materialdata"));
		AssetManager::emplaceMaterial(materialData);
	}

	MaterialSystem::Material MaterialSystem::loadMaterial(const std::string& loadLoc, bool ignoreWarns) {
		std::ifstream stream(loadLoc);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		if (!data["WBASSET_Material"]) return Material();


		if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
			SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect engine version");
		}// change this to check if the version is less or more
		if (data["WorldBuilder3D"].as<std::string>() != EDITOR_VERSION.toString()) {
			SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect editor version");
		}// change this to check if the version is less or more

		std::string levelName = data["Level"].as<std::string>();

		if (data["Material"]) {
			auto container = data["Container"];
			Material loadedMaterial{};

			SHARD3D_INFO("Loading material with ID {0}", data["MaterialID"].as<uint64_t>());

			if (container["Physics"]["MaterialStruct"].as<std::string>() != "SurfaceMaterial") return Material();
			loadedMaterial.materialTag =  data["WBASSET_Material"].as<std::string>();
			loadedMaterial.guid = data["MaterialID"].as<uint64_t>();

			loadedMaterial.type = (MaterialType)container["Physics"]["MaterialStruct"].as<uint32_t>();
			loadedMaterial.surfaceMaterial.surfaceMat = (SurfaceMaterialType)container["Physics"]["MaterialType"].as<uint32_t>();
			loadedMaterial.surfaceMaterial.surfaceProp = (SurfaceMaterialProperties)container["Physics"]["MaterialProperties"].as<uint32_t>();
			
			loadedMaterial.surfaceMaterial.drawData.culling = (VkCullModeFlagBits)container["Draw"]["culling"].as<uint32_t>();
			loadedMaterial.surfaceMaterial.drawData.polygonMode = (VkPolygonMode)container["Draw"]["polygonMode"].as<uint32_t>();

			loadedMaterial.surfaceMaterial.diffuseColor = container["Data"]["diffuseVec4"].as<glm::vec4>();
			loadedMaterial.surfaceMaterial.specular = container["Data"]["specularFloat"].as<float>();
			loadedMaterial.surfaceMaterial.roughness = container["Data"]["roughnessFloat"].as<float>();
			loadedMaterial.surfaceMaterial.metallic = container["Data"]["metallicFloat"].as<float>();

			loadedMaterial.surfaceMaterial.normalTex.path = container["Data"]["normalTex"].as<std::string>();
			loadedMaterial.surfaceMaterial.diffuseTex.path = container["Data"]["diffuseTex"].as<std::string>();
			loadedMaterial.surfaceMaterial.specularTex.path = container["Data"]["specularTex"].as<std::string>();
			loadedMaterial.surfaceMaterial.roughnessTex.path = container["Data"]["roughnessTex"].as<std::string>();
			loadedMaterial.surfaceMaterial.metallicTex.path = container["Data"]["metallicTex"].as<std::string>();

			// this is strictly a local path, which means all materials must be in this folder
			loadedMaterial.path = loadLoc;//.substr(loadLoc.rfind("assets\\materialdata"));
			AssetManager::emplaceMaterial(loadedMaterial);
			return loadedMaterial;
		}
	}
	MaterialSystem::MaterialList MaterialSystem::loadList(const std::string& loadLoc, bool ignoreWarns) {
		std::ifstream stream(loadLoc);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		if (!data["WBASSET_MaterialList"]) return MaterialList();


		if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
			SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect engine version");
		}// change this to check if the version is less or more
		if (data["WorldBuilder3D"].as<std::string>() != EDITOR_VERSION.toString()) {
			SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect editor version");
		}// change this to check if the version is less or more

		std::string levelName = data["Level"].as<std::string>();

		if (data["List"]) {
			MaterialList loadedList{};

			SHARD3D_INFO("Loading material list with ID {0}", data["MaterialListID"].as<uint64_t>());

			loadedList.materialListTag = data["WBASSET_MaterialList"].as<std::string>();
			loadedList.guid = data["MaterialListID"].as<uint64_t>();

			for (auto material : data["Material"]) {
				loadedList.list.push_back(loadMaterial(material["Location"].as<std::string>(), true));
			}
			return loadedList;
		}
	}
	void MaterialSystem::createDescriptorSetLayout() {
		
	}

}