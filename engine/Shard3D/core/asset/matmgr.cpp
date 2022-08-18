#include "../../s3dpch.h"

#include "matmgr.h" 

#include <fstream>


namespace Shard3D {
	
	MaterialManager::MaterialManager(SurfaceMaterial& material){}

	static void saveMaterial_SurfaceMaterial_ShadedOpaque(YAML::Emitter& out, SurfaceMaterial_ShadedOpaque* material) {
		out << YAML::BeginMap;
		out << YAML::Key << "diffuseVec4" << YAML::Value << material->diffuseColor;
		out << YAML::Key << "diffuseTex" << YAML::Value << material->diffuseTex;
		out << YAML::Key << "specularFloat" << YAML::Value << material->specular;
		out << YAML::Key << "specularTex" << YAML::Value << material->specularTex;
		out << YAML::Key << "roughnessFloat" << YAML::Value << material->shininess;
		out << YAML::Key << "roughnessTex" << YAML::Value << material->shininessTex;
		out << YAML::Key << "metallicFloat" << YAML::Value << material->metallic;
		out << YAML::Key << "metallicTex" << YAML::Value << material->metallicTex;

		out << YAML::Key << "normalTex" << YAML::Value << material->normalTex;
		out << YAML::EndMap;
	}
	//static void saveMaterial_SurfaceMaterial_ShadedMasked(YAML::Emitter& out, SurfaceMaterial_ShadedMasked* material) {
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "diffuseVec4" << YAML::Value << material->diffuseColor;
	//	out << YAML::Key << "diffuseTex" << YAML::Value << material->diffuseTex;
	//	out << YAML::Key << "specularFloat" << YAML::Value << material->specular;
	//	out << YAML::Key << "specularTex" << YAML::Value << material->specularTex;
	//	out << YAML::Key << "roughnessFloat" << YAML::Value << material->shininess;
	//	out << YAML::Key << "roughnessTex" << YAML::Value << material->shininessTex;
	//	out << YAML::Key << "metallicFloat" << YAML::Value << material->metallic;
	//	out << YAML::Key << "metallicTex" << YAML::Value << material->metallicTex;
	//
	//	out << YAML::Key << "normalTex" << YAML::Value << material->normalTex;
	//	out << YAML::Key << "maskTex" << YAML::Value << material->maskTex;
	//	out << YAML::EndMap;
	//}

	void MaterialManager::saveMaterial(const sPtr<SurfaceMaterial>& material, const std::string& destPath, bool ignoreWarns) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
		out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION.toString();
		out << YAML::Key << "WBASSET_Material" << YAML::Value << YAML::BeginSeq;
		out << YAML::BeginMap;
		out << YAML::Key << "Material" << YAML::Value << material->materialTag;

		out << YAML::Key << "Physics";
		out << YAML::BeginMap;
		const volatile char* surfaceMaterialType = typeid(*material.get()).name();
		out << YAML::Key << "MaterialType" << YAML::Value << "SurfaceMaterial";
		out << YAML::Key << "MaterialClass" << YAML::Value << typeid(*material.get()).name();
		out << YAML::EndMap;
			
		out << YAML::Key << "Draw";
		out << YAML::BeginMap;
		out << YAML::Key << "VkCullModeFlags" << YAML::Value << material->drawData.culling;
		out << YAML::Key << "VkPolygonMode" << YAML::Value << material->drawData.polygonMode;
		out << YAML::EndMap;
		
		out << YAML::Key << "Data";
		//material->serialize(destPath);
		if (surfaceMaterialType == typeid(SurfaceMaterial_ShadedOpaque).name()) {}
		//	material->serialize(destPath);
		//} else if (surfaceMaterialType == typeid(SurfaceMaterial_ShadedMasked).name()) {
		//	saveMaterial_SurfaceMaterial_ShadedMasked(out, reinterpret_cast<SurfaceMaterial_ShadedMasked*>(material.get()));
		//}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		auto newPath = destPath;
		if (!strUtils::hasEnding(destPath, ".wbasset")) newPath = destPath + ".wbasset";
		std::ofstream fout(newPath);
		fout << out.c_str();
		fout.flush();
		fout.close();
	}
	sPtr<SurfaceMaterial> MaterialManager::loadSurfaceMaterial(const std::string& sourcePath, bool ignoreWarns) {
		
		return make_sPtr<SurfaceMaterial_ShadedOpaque>();
		std::ifstream stream(sourcePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		
		YAML::Node data = YAML::Load(strStream.str());
		
		if (!data["WBASSET_Material"]) return make_sPtr<SurfaceMaterial_ShadedOpaque>();
		
		
		if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
			SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect engine version");
		}// change this to check if the version is less or more
		if (data["WorldBuilder3D"].as<std::string>() != EDITOR_VERSION.toString()) {
			SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect editor version");
		}// change this to check if the version is less or more
		
		std::string levelName = data["Level"].as<std::string>();
		
		if (data["Material"]) {
			auto container = data["Material"];

			DrawData drawData;
			drawData.culling = static_cast<VkCullModeFlags>(container["Draw"]["culling"].as<uint32_t>());
			drawData.polygonMode = static_cast<VkPolygonMode>(container["Draw"]["polygonMode"].as<uint32_t>());

			if (container["Physics"]["MaterialType"].as<std::string>() != "SurfaceMaterial") return make_sPtr<SurfaceMaterial_ShadedOpaque>();

			if (container["Physics"]["MaterialClass"].as<std::string>() == typeid(SurfaceMaterial_ShadedOpaque).name()) {

			}// else if (container["Physics"]["MaterialClass"].as<std::string>() == typeid(SurfaceMaterial_ShadedMasked).name()) {
				
			//}
			
		//loadedMaterial.surfaceMaterial.diffuseColor = container["Data"]["diffuseVec4"].as<glm::vec4>();
		//loadedMaterial.surfaceMaterial.specular = container["Data"]["specularFloat"].as<float>();
		//loadedMaterial.surfaceMaterial.roughness = container["Data"]["roughnessFloat"].as<float>();
		//loadedMaterial.surfaceMaterial.metallic = container["Data"]["metallicFloat"].as<float>();
		//
		//loadedMaterial.surfaceMaterial.normalTex.path = container["Data"]["normalTex"].as<std::string>();
		//loadedMaterial.surfaceMaterial.diffuseTex.path = container["Data"]["diffuseTex"].as<std::string>();
		//loadedMaterial.surfaceMaterial.specularTex.path = container["Data"]["specularTex"].as<std::string>();
		//loadedMaterial.surfaceMaterial.roughnessTex.path = container["Data"]["roughnessTex"].as<std::string>();
		//loadedMaterial.surfaceMaterial.metallicTex.path = container["Data"]["metallicTex"].as<std::string>();
		//
		//// this is strictly a local path, which means all materials must be in this folder
		//loadedMaterial.path = loadLoc;//.substr(loadLoc.rfind("assets\\materialdata"));
		//AssetManager::emplaceMaterial(loadedMaterial);
		//	return loadedMaterial;
		}

	}	
}