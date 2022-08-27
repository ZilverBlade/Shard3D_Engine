#include "../../s3dpch.h"

#include "matmgr.h" 

#include <fstream>


namespace Shard3D {
	
	MaterialManager::MaterialManager(SurfaceMaterial& material) {}
	
	void MaterialManager::saveMaterial(const rPtr<SurfaceMaterial>& material, const std::string& destPath, bool ignoreWarns) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
		out << YAML::Key << "AssetType" << YAML::Value << "surface_material";
		out << YAML::Key << "SurfaceMaterial" << YAML::Value << YAML::BeginSeq;
		out << YAML::BeginMap;
		out << YAML::Key << "Material" << YAML::Value << material->materialTag;

		out << YAML::Key << "Physics";
		out << YAML::BeginMap;
		out << YAML::Key << "MaterialType" << YAML::Value << "SurfaceMaterial";
		out << YAML::Key << "MaterialClass" << YAML::Value << typeid(*material.get()).name();
		out << YAML::EndMap;
			
		out << YAML::Key << "Draw";
		out << YAML::BeginMap;
		out << YAML::Key << "VkCullModeFlags" << YAML::Value << material->drawData.culling;
		out << YAML::Key << "VkPolygonMode" << YAML::Value << material->drawData.polygonMode;
		out << YAML::EndMap;
		
		out << YAML::Key << "Data";

		material->serialize(&out);

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::string newPath = destPath;
		if (!strUtils::hasEnding(destPath, ".s3dasset")) newPath = destPath + ".s3dasset";
		std::ofstream fout(newPath);
		fout << out.c_str();
		fout.flush();
		fout.close();
	}
	rPtr<SurfaceMaterial> MaterialManager::loadSurfaceMaterial(const AssetID& asset, bool ignoreWarns) {
		std::ifstream stream(asset.getFile());
		std::stringstream strStream;
		strStream << stream.rdbuf();
		
		YAML::Node data = YAML::Load(strStream.str());
		
		if (data["AssetType"].as<std::string>() != "surface_material") return make_rPtr<SurfaceMaterial_ShadedOpaque>();
		
		if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
			SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect engine version");
		}
		
		if (data["SurfaceMaterial"]) {
			auto container = data["SurfaceMaterial"][0];
			DrawData drawData;
			drawData.culling = static_cast<VkCullModeFlags>(container["Draw"]["VkCullModeFlags"].as<uint32_t>());
			drawData.polygonMode = static_cast<VkPolygonMode>(container["Draw"]["VkPolygonMode"].as<uint32_t>());

			if (container["Physics"]["MaterialClass"].as<std::string>() == typeid(SurfaceMaterial_ShadedOpaque).name()) {
				rPtr<SurfaceMaterial_ShadedOpaque> material = make_rPtr<SurfaceMaterial_ShadedOpaque>();
				material->materialTag = container["Material"].as<std::string>();
				material->drawData = drawData;
				material->deserialize(&container);
				return material;
			}
			if (container["Physics"]["MaterialClass"].as<std::string>() == typeid(SurfaceMaterial_ShadedMasked).name()) {
				rPtr<SurfaceMaterial_ShadedMasked> material = make_rPtr<SurfaceMaterial_ShadedMasked>();
				material->materialTag = container["Material"].as<std::string>();
				material->drawData = drawData;
				material->deserialize(&container);
				return material;
			}
			if (container["Physics"]["MaterialClass"].as<std::string>() == typeid(SurfaceMaterial_ShadedTranslucent).name()) {
				rPtr<SurfaceMaterial_ShadedTranslucent> material = make_rPtr<SurfaceMaterial_ShadedTranslucent>();
				material->materialTag = container["Material"].as<std::string>();
				material->drawData = drawData;
				material->deserialize(&container);
				return material;
			}
		}
		SHARD3D_ERROR("Unable to load material {0} (ID {1}))", asset.getFile(), asset.getID());
		return make_rPtr<SurfaceMaterial_ShadedOpaque>();
	}	
}