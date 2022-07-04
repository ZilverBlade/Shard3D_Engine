#include "bpmgr.hpp"

#include "actor.hpp" 
#include "../components.hpp"
#include "../wb3d/wb3d_imgui_frame.hpp"
#include "../utils/definitions.hpp"
#include "../utils/engine_utils.hpp"
#include "../utils/yaml_ext.hpp"

#include <fstream>
#include "level.hpp" // for the killMesh function
#include "assetmgr.hpp"

namespace Shard3D {
	namespace wb3d {
		BlueprintManager::BlueprintManager(Actor& mActor) : actor(mActor) {
			SHARD3D_INFO("Loading Blueprint Manager");
		}

		std::string BlueprintManager::encrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string encryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				encryptedString.push_back((char)
					((((c + LEVEL_CIPHER_KEY) * 2) - LEVEL_CIPHER_KEY) / 2));
			}

			SHARD3D_LOG("Duration of Blueprint encryption: {0} ms", std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000);
			return encryptedString;
		}
		std::string BlueprintManager::decrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string decryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				decryptedString.push_back((char)
					(((c * 2) + LEVEL_CIPHER_KEY) / 2) - LEVEL_CIPHER_KEY);
			}

			SHARD3D_LOG("Duration of Blueprint decryption: {0} ms", std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000);
			return decryptedString;
		}		

		static void saveActor(YAML::Emitter& out, Actor actor) {
			out << YAML::BeginMap;
			// TRANSFORM
			if (actor.hasComponent<Components::TransformComponent>()) {
				out << YAML::Key << "TransformComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "Translation" << YAML::Value << actor.getComponent<Components::TransformComponent>().translation;
					out << YAML::Key << "Rotation" << YAML::Value << actor.getComponent<Components::TransformComponent>().rotation;
					out << YAML::Key << "Scale" << YAML::Value << actor.getComponent<Components::TransformComponent>().scale;
				out << YAML::EndMap;
			}

			// CAMERA
			if (actor.hasComponent<Components::CameraComponent>()) {
				out << YAML::Key << "CameraComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "ProjectionType" << YAML::Value << (int)actor.getComponent<Components::CameraComponent>().projectionType;
				out << YAML::Key << "FOV" << YAML::Value << actor.getComponent<Components::CameraComponent>().fov;
				out << YAML::Key << "NearClipPlane" << YAML::Value << actor.getComponent<Components::CameraComponent>().nearClip;
				out << YAML::Key << "FarClipPlane" << YAML::Value << actor.getComponent<Components::CameraComponent>().farClip;
				out << YAML::Key << "AspectRatio" << YAML::Value << actor.getComponent<Components::CameraComponent>().ar;
				out << YAML::EndMap;
			}

			// MODEL
			if (actor.hasComponent<Components::MeshComponent>()) {
				out << YAML::Key << "MeshComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "MeshPath" << YAML::Value << actor.getComponent<Components::MeshComponent>().file;
					out << YAML::Key << "MeshFormat" << YAML::Value << (int)actor.getComponent<Components::MeshComponent>().type;
				out << YAML::EndMap;
			}

			if (actor.hasComponent<Components::PointlightComponent>()) {
				out << YAML::Key << "PointlightComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "Emission" << YAML::Value << glm::vec4(actor.getComponent<Components::PointlightComponent>().color, actor.getComponent<Components::PointlightComponent>().lightIntensity);
					out << YAML::Key << "Radius" << YAML::Value << actor.getComponent<Components::PointlightComponent>().radius;
					out << YAML::Key << "Specularness" << YAML::Value << actor.getComponent<Components::PointlightComponent>().specularMod;
					out << YAML::Key << "AttenuationCLQ" << YAML::Value << actor.getComponent<Components::PointlightComponent>().attenuationMod;
				out << YAML::EndMap;
			}

			if (actor.hasComponent<Components::SpotlightComponent>()) {
				out << YAML::Key << "SpotlightComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "Emission" << YAML::Value << glm::vec4(actor.getComponent<Components::SpotlightComponent>().color, actor.getComponent<Components::SpotlightComponent>().lightIntensity);
					out << YAML::Key << "Radius" << YAML::Value << actor.getComponent<Components::SpotlightComponent>().radius;
					out << YAML::Key << "Angle" << YAML::Value << glm::vec2(actor.getComponent<Components::SpotlightComponent>().innerAngle, actor.getComponent<Components::SpotlightComponent>().outerAngle);
					out << YAML::Key << "Specularness" << YAML::Value << actor.getComponent<Components::SpotlightComponent>().specularMod;
					out << YAML::Key << "AttenuationCLQ" << YAML::Value << actor.getComponent<Components::SpotlightComponent>().attenuationMod;
				out << YAML::EndMap;
			}

			if (actor.hasComponent<Components::DirectionalLightComponent>()) {
				out << YAML::Key << "DirectionalLightComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "Emission" << YAML::Value << glm::vec4(actor.getComponent<Components::DirectionalLightComponent>().color, actor.getComponent<Components::DirectionalLightComponent>().lightIntensity);
					out << YAML::Key << "Specularness" << YAML::Value << actor.getComponent<Components::DirectionalLightComponent>().specularMod;
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}


		void BlueprintManager::removeAllComponents(Actor actor) {
			//don't remove 3 crutial components like transform, tag and GUID since they must be controllable on any actor
			if (actor.hasComponent<Components::CameraComponent>()) actor.killComponent<Components::CameraComponent>();
			if (actor.hasComponent<Components::CppScriptComponent>()) actor.killComponent<Components::CppScriptComponent>();
			if (actor.hasComponent<Components::DirectionalLightComponent>()) actor.killComponent<Components::DirectionalLightComponent>();
			if (actor.hasComponent<Components::MeshComponent>()) actor.eLevel->killMesh(actor);
			if (actor.hasComponent<Components::PointlightComponent>()) actor.killComponent<Components::PointlightComponent>();
			if (actor.hasComponent<Components::SpotlightComponent>()) actor.killComponent<Components::SpotlightComponent>();
		}

		void BlueprintManager::convert(const std::string& destinationPath, Blueprint blueprint, bool encryptLevel) {
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION;
			out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION;
			out << YAML::Key << "WBASSET_Blueprint" << YAML::Value << blueprint.name;
			out << YAML::Key << "BlueprintID" << YAML::Value << blueprint.getGUID();
			out << YAML::Key << "Container" << YAML::Value << YAML::BeginSeq;

			saveActor(out, actor);

			out << YAML::EndSeq;
			out << YAML::EndMap;
			std::string newPath = destinationPath;
			if (!strUtils::hasEnding(destinationPath, ".wbasset")) newPath = destinationPath + ".wbasset";
			if (encryptLevel) {
				std::ofstream fout(newPath);
				fout << encrypt(out.c_str());
				fout.flush();
				fout.close();
			}
			else {
				std::ofstream fout(newPath);
				fout << out.c_str();
				fout.flush();
				fout.close();
			}
			removeAllComponents(actor);// this is strictly a local path, which means all blueprints must be in this folder
			blueprint.assetFile = newPath.substr(newPath.rfind("assets\\blueprintdata")); 
			SHARD3D_LOG("Saved blueprint '{0}'", newPath);
		}

		void BlueprintManager::convertRuntime(const std::string& destinationPath) {
			assert(false);
		}

		BlueprintMgrResults BlueprintManager::load(const std::string& sourcePath, bool ignoreWarns) {
			std::ifstream stream(sourcePath);
			std::stringstream strStream;
			strStream << stream.rdbuf();

#ifdef GAME_RELEASE_READY
			YAML::Node data = YAML::Load(decrypt(strStream.str()));
#if BETA_DEBUG_TOOLS
			std::ofstream fout(sourcePath + ".dcr");
			fout << decrypt(strStream.str());
			fout.flush();
			fout.close();
#endif
#else
			YAML::Node data = YAML::Load(strStream.str());
#endif
			
			if (ignoreWarns == false) {
				if (!data["WBASSET_Blueprint"]) return BlueprintMgrResults::WrongFileResult;

				if (data["Shard3D"].as<std::string>() != ENGINE_VERSION) { 
					SHARD3D_WARN("Incorrect engine version");
					return BlueprintMgrResults::OldEngineVersionResult;
				}// change this to check if the version is less or more
				if (data["WorldBuilder3D"].as<std::string>() != EDITOR_VERSION) {
					SHARD3D_WARN("Incorrect editor version");
					return BlueprintMgrResults::OldEditorVersionResult;
				}// change this to check if the version is less or more

			}
			
			if (data["Container"]) {
				for (auto actor : data["Container"]) {
					Actor loadedActor{};

					SHARD3D_LOG("Building blueprint with ID {0}", actor["BlueprintID"].as<uint64_t>());
					// Dont load actor if no TagComponent, every actor should have a TagComponent, so if an actor has no TagComponent, it must be some kind of core thing

					if (actor["TransformComponent"]) {
#ifndef ACTOR_FORCE_TRANSFORM_COMPONENT		
						loadedActor.addComponent<Components::TransformComponent>();
#endif
						loadedActor.getComponent<Components::TransformComponent>().translation = actor["TransformComponent"]["Translation"].as<glm::vec3>();
						loadedActor.getComponent<Components::TransformComponent>().rotation = actor["TransformComponent"]["Rotation"].as<glm::vec3>();
						loadedActor.getComponent<Components::TransformComponent>().scale = actor["TransformComponent"]["Scale"].as<glm::vec3>();

					}

					// CAMERA
					if (actor["CameraComponent"]) {
						loadedActor.addComponent<Components::CameraComponent>();

						loadedActor.getComponent<Components::CameraComponent>().projectionType = (Components::CameraComponent::ProjectType)actor["CameraComponent"]["ProjectionType"].as<int>();
						loadedActor.getComponent<Components::CameraComponent>().fov = actor["CameraComponent"]["FOV"].as<float>();
						loadedActor.getComponent<Components::CameraComponent>().nearClip = actor["CameraComponent"]["NearClipPlane"].as<float>();
						loadedActor.getComponent<Components::CameraComponent>().farClip = actor["CameraComponent"]["FarClipPlane"].as<float>();
						loadedActor.getComponent<Components::CameraComponent>().ar = actor["CameraComponent"]["AspectRatio"].as<float>();
					}

					if (actor["MeshComponent"]) {
						loadedActor.addComponent<Components::MeshComponent>(
							actor["MeshComponent"]["MeshPath"].as<std::string>());
					}
					
					if (actor["PointlightComponent"]) {
						loadedActor.addComponent<Components::PointlightComponent>();

						loadedActor.getComponent<Components::PointlightComponent>().color = actor["PointlightComponent"]["Emission"].as<glm::vec3>();
						loadedActor.getComponent<Components::PointlightComponent>().lightIntensity = actor["PointlightComponent"]["Emission"].as<glm::vec4>().w;
						loadedActor.getComponent<Components::PointlightComponent>().radius = actor["PointlightComponent"]["Radius"].as<float>();
						loadedActor.getComponent<Components::PointlightComponent>().specularMod = actor["PointlightComponent"]["Specularness"].as<float>();
						loadedActor.getComponent<Components::PointlightComponent>().attenuationMod = actor["PointlightComponent"]["AttenuationCLQ"].as<glm::vec3>();
					}
					if (actor["SpotlightComponent"]) {
						loadedActor.addComponent<Components::SpotlightComponent>();

						loadedActor.getComponent<Components::SpotlightComponent>().color = actor["SpotlightComponent"]["Emission"].as<glm::vec3>();
						loadedActor.getComponent<Components::SpotlightComponent>().lightIntensity = actor["SpotlightComponent"]["Emission"].as<glm::vec4>().w;
						loadedActor.getComponent<Components::SpotlightComponent>().radius = actor["SpotlightComponent"]["Radius"].as<float>();
						loadedActor.getComponent<Components::SpotlightComponent>().innerAngle = actor["SpotlightComponent"]["Angle"].as<glm::vec2>().x;
						loadedActor.getComponent<Components::SpotlightComponent>().outerAngle = actor["SpotlightComponent"]["Angle"].as<glm::vec2>().y;
						loadedActor.getComponent<Components::SpotlightComponent>().specularMod = actor["SpotlightComponent"]["Specularness"].as<float>();
						loadedActor.getComponent<Components::SpotlightComponent>().attenuationMod = actor["SpotlightComponent"]["AttenuationCLQ"].as<glm::vec3>();
					}
					if (actor["DirectionalLightComponent"]) {
						loadedActor.addComponent<Components::DirectionalLightComponent>();

						loadedActor.getComponent<Components::DirectionalLightComponent>().color = actor["DirectionalLightComponent"]["Emission"].as<glm::vec3>();
						loadedActor.getComponent<Components::DirectionalLightComponent>().lightIntensity = actor["DirectionalLightComponent"]["Emission"].as<glm::vec4>().w;
						loadedActor.getComponent<Components::DirectionalLightComponent>().specularMod = actor["DirectionalLightComponent"]["Specularness"].as<float>();
					}
				}
			}
			return BlueprintMgrResults::SuccessResult;
		}



		BlueprintMgrResults BlueprintManager::loadRuntime(const std::string& sourcePath) {
			assert(false);
			return BlueprintMgrResults::ErrorResult;
		}
	}
}