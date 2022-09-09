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

	static void PPOSerializeMyParam(YAML::Emitter& out, RandomPPOParam param, std::string name) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		out << (uint32_t)param.getType();
		out << name;
		out << YAML::Binary(reinterpret_cast<uint8_t*>(param.get()), param.getCPUSize());
		out << YAML::EndSeq;
	}

	void MaterialManager::saveMaterial(const rPtr<PostProcessingMaterial>& material, const std::string& destPath, bool ignoreWarns) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
		out << YAML::Key << "AssetType" << YAML::Value << "postprocessing_material";
		out << YAML::Key << "PPOMaterial" << YAML::Value << YAML::BeginSeq;
		out << YAML::BeginMap;
		out << YAML::Key << "Material" << YAML::Value << material->materialTag;
		out << YAML::Key << "Data" << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << material->shaderPath;
		out << YAML::Key << "MyParams" << YAML::Value << YAML::Flow << YAML::BeginSeq;
		for (int i = 0; i < material->myParams.size(); i++) {
			PPOSerializeMyParam(out, material->myParams[i], material->myParamNames[i]);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
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
			if (data["AssetType"].as<std::string>() != "surface_material") { SHARD3D_ERROR("Material {0} (ID {1})) is not a surface material!", asset.getFile(), asset.getID()); return make_rPtr<SurfaceMaterial_Shaded>(); }

			if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
				SHARD3D_WARN("Different engine version, material may not be loaded properly");
			}

			if (data["SurfaceMaterial"]) {
				auto container = data["SurfaceMaterial"][0];
				_DrawData drawData;
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
	rPtr<PostProcessingMaterial> MaterialManager::loadPPOMaterial(const AssetID& asset, bool ignoreWarns) {
		std::ifstream stream(asset.getFile());

		if (!stream.good()) return make_rPtr<PostProcessingMaterial>();

		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		try 
		{
			if (data["AssetType"].as<std::string>() != "postprocessing_material") { SHARD3D_ERROR("Material {0} (ID {1})) is not a post processing material!", asset.getFile(), asset.getID()); return make_rPtr<PostProcessingMaterial>(); }

			if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
				SHARD3D_WARN("Different engine version, material may not be loaded properly");
			}

			if (data["PPOMaterial"]) {
				// find a way to serialise all of the buffer data
				rPtr<PostProcessingMaterial> material = make_rPtr<PostProcessingMaterial>();
				material->materialTag = data["PPOMaterial"][0]["Material"].as<std::string>();
				auto container = data["PPOMaterial"][0]["Data"];
				material->shaderPath = container["Shader"].as<std::string>();
				for (int i = 0; i < container["MyParams"].size(); i++) {
					auto myParam = container["MyParams"][i];
					PPO_Types type = (PPO_Types)myParam[0].as<uint32_t>();
					switch (type) {
						case(PPO_Types::Int32):
							material->addParameter(*reinterpret_cast<const int*>(myParam[2].as<YAML::Binary>().data()), myParam[1].as<std::string>());
							break;
						case(PPO_Types::Float):
							material->addParameter(*reinterpret_cast<const float*>(myParam[2].as<YAML::Binary>().data()), myParam[1].as<std::string>());
							break;
						case(PPO_Types::Float2):
							material->addParameter(*reinterpret_cast<const glm::vec2*>(myParam[2].as<YAML::Binary>().data()), myParam[1].as<std::string>());
							break;
						case(PPO_Types::Float4):
							material->addParameter(*reinterpret_cast<const glm::vec4*>(myParam[2].as<YAML::Binary>().data()), myParam[1].as<std::string>());
							break;
					}
				}
				return material;
			}
		}
		catch (YAML::Exception ex) {
			SHARD3D_ERROR("Unable to load material {0} (ID {1})). Reason: {2}", asset.getFile(), asset.getID(), ex.msg);
		}
		return make_rPtr<PostProcessingMaterial>();
	}

}