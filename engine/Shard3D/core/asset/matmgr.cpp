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
		out << YAML::Key << "Class" << YAML::Value << typeid(*material.get()).name();
		out << YAML::Key << "BlendMode" << YAML::Value << material->getBlendMode();
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
		
		if (!stream.good())return make_rPtr<SurfaceMaterial_Shaded>();

		std::stringstream strStream;
		strStream << stream.rdbuf();
			
		YAML::Node data = YAML::Load(strStream.str());
		try {
			if (data["AssetType"].as<std::string>() != "surface_material") return make_rPtr<SurfaceMaterial_Shaded>();

			if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
				SHARD3D_WARN("(Shard3D::MaterialSystem) Incorrect engine version");
			}

			if (data["SurfaceMaterial"]) {
				auto container = data["SurfaceMaterial"][0];
				DrawData drawData;
				drawData.culling = static_cast<VkCullModeFlags>(container["Draw"]["VkCullModeFlags"].as<uint32_t>());
				drawData.polygonMode = static_cast<VkPolygonMode>(container["Draw"]["VkPolygonMode"].as<uint32_t>());

				if (container["Physics"]["Class"].as<std::string>() == typeid(SurfaceMaterial_Shaded).name()) {
					rPtr<SurfaceMaterial_Shaded> material = make_rPtr<SurfaceMaterial_Shaded>();
					material->setBlendMode(container["Physics"]["BlendMode"].as<uint32_t>());
					material->materialTag = container["Material"].as<std::string>();
					material->drawData = drawData;
					material->deserialize(&container);
					return material;
				}
			}
		}
		catch (YAML::Exception ex) {
			SHARD3D_ERROR("Unable to load material {0} (ID {1})). Reason: {2}", asset.getFile(), asset.getID(), ex.msg);
		}
		return make_rPtr<SurfaceMaterial_Shaded>();
	}	
}