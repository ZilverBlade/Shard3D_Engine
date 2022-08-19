#include "../../s3dpch.h"
#include "levelmgr.h"

#include "../../ecs.h"
#include "../../core.h"
#include "../asset/assetmgr.h"
#include <fstream>
namespace Shard3D {
	namespace ECS {
		LevelManager::LevelManager(const sPtr<Level>& level) : mLevel(level) { 
			SHARD3D_INFO("Loading Level Manager");
		}

		std::string LevelManager::encrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string encryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				encryptedString.push_back((char)
					((((c + WB3D_CIPHER_KEY) * 2) - WB3D_CIPHER_KEY) / 2));
			}

			SHARD3D_LOG("Duration of Level encryption: {0} ms", std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000);
			return encryptedString;
		}
		std::string LevelManager::decrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string decryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				decryptedString.push_back((char)
					(((c * 2) + WB3D_CIPHER_KEY) / 2) - WB3D_CIPHER_KEY);
			}

			SHARD3D_LOG("Duration of Level decryption: {0} ms", std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000);
			return decryptedString;
		}

		static void saveActor(YAML::Emitter& out, Actor actor) {
			if (actor.isInvalid()) return;
			out << YAML::BeginMap;
			out << YAML::Key << "Actor" << YAML::Value << actor.getUUID();

			// TAG
			if (actor.hasComponent<Components::TagComponent>()) {
				out << YAML::Key << "TagComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "Tag" << YAML::Value << actor.getTag();
				out << YAML::EndMap;
			}

			// TRANSFORM
			if (actor.hasComponent<Components::TransformComponent>()) {
				out << YAML::Key << "TransformComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "Translation" << YAML::Value << actor.getComponent<Components::TransformComponent>().getTranslation();
					out << YAML::Key << "Rotation" << YAML::Value << actor.getComponent<Components::TransformComponent>().getRotation();
					out << YAML::Key << "Scale" << YAML::Value << actor.getComponent<Components::TransformComponent>().getScale();
				out << YAML::EndMap;
			}

			// SCRIPT
			if (actor.hasComponent<Components::ScriptComponent>()) {
				out << YAML::Key << "ScriptComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "Module" << YAML::Value << actor.getComponent<Components::ScriptComponent>().name;
				out << YAML::Key << "Language" << YAML::Value << (!actor.getComponent<Components::ScriptComponent>().lang? "C#" : "VB");
				out << YAML::EndMap;
			}

			// CAMERA
			if (actor.hasComponent<Components::CameraComponent>()) {
				out << YAML::Key << "CameraComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "ProjectionType" << YAML::Value << (int)actor.getComponent<Components::CameraComponent>().getProjectionType();
				out << YAML::Key << "FOV" << YAML::Value << actor.getComponent<Components::CameraComponent>().getFOV();
				out << YAML::Key << "NearClipPlane" << YAML::Value << actor.getComponent<Components::CameraComponent>().getNearClip();
				out << YAML::Key << "FarClipPlane" << YAML::Value << actor.getComponent<Components::CameraComponent>().getFarClip();
				out << YAML::Key << "AspectRatio" << YAML::Value << actor.getComponent<Components::CameraComponent>().ar;
				out << YAML::EndMap;
			}
			// AUDIO
			if (actor.hasComponent<Components::AudioComponent>()) {
				out << YAML::Key << "AudioComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "File" << YAML::Value << actor.getComponent<Components::AudioComponent>().file;
				out << YAML::Key << "Properties";
				out << YAML::BeginMap;
				out << YAML::Key << "Volume" << YAML::Value << actor.getComponent<Components::AudioComponent>().properties.volume;
				out << YAML::Key << "Pitch" << YAML::Value << actor.getComponent<Components::AudioComponent>().properties.pitch;
				out << YAML::EndMap; 
				out << YAML::EndMap;
			}
			// BILLBOARD
			if (actor.hasComponent<Components::BillboardComponent>()) {
				out << YAML::Key << "BillboardComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "TexPath" << YAML::Value << actor.getComponent<Components::BillboardComponent>().file;
				out << YAML::Key << "BillboardOrientation" << YAML::Value << (int)actor.getComponent<Components::BillboardComponent>().orientation;
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
		void LevelManager::save(const std::string& destinationPath, bool encryptLevel) {
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
			out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION.toString();
			out << YAML::Key << "Level" << YAML::Value << "Some kind of level";
			out << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;

			mLevel->registry.each([&](auto actorGUID) { Actor actor = { actorGUID, mLevel.get() };
				if (!actor) return;
				saveActor(out, actor);
			});

			out << YAML::EndSeq;
			out << YAML::EndMap;
			std::string newPath = destinationPath;
			if (!strUtils::hasEnding(destinationPath, ".wbl")) newPath = destinationPath + ".wbl";
			if (encryptLevel) {
				std::ofstream fout(newPath);
				fout << encrypt(out.c_str());
				fout.flush();
				fout.close();
			} else {
				std::ofstream fout(newPath);
				fout << out.c_str();
				fout.flush();
				fout.close();
			}
		
			SHARD3D_LOG("Saved scene '{0}'", newPath);
		}

		void LevelManager::saveRuntime(const std::string& destinationPath) {
			SHARD3D_ASSERT(false);
		}

		LevelMgrResults LevelManager::load(const std::string& sourcePath, bool ignoreWarns) {
			AssetManager::clearAllAssets(); // remove since new stuff will be loaded into memory
			std::ifstream stream(sourcePath);
			std::stringstream strStream;
			strStream << stream.rdbuf();

#ifdef _DEPLOY
			//YAML::Node data = YAML::Load(decrypt(strStream.str()));
			YAML::Node data = YAML::Load(strStream.str());
#ifdef ENSET_BETA_DEBUG_TOOLS
			std::ofstream fout(sourcePath + ".dcr");
			fout << decrypt(strStream.str());
			fout.flush();
			fout.close();
#endif
#else
			YAML::Node data = YAML::Load(strStream.str());
#endif

			if (ignoreWarns == false) {
				if (!data["Level"]) return LevelMgrResults::WrongFileResult;

				if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
					SHARD3D_WARN("Incorrect engine version");
					return LevelMgrResults::OldEngineVersionResult;
				}// change this to check if the version is less or more
				if (data["WorldBuilder3D"].as<std::string>() != EDITOR_VERSION.toString()) {
					SHARD3D_WARN("Incorrect editor version");
					return LevelMgrResults::OldEditorVersionResult;
				}// change this to check if the version is less or more

			}

			std::string levelName = data["Level"].as<std::string>();
			
			if (data["Actors"]) {
				for (auto actor : data["Actors"]) {
					Actor loadedActor{};

					SHARD3D_INFO("Loading actor with ID {0}", actor["Actor"].as<uint64_t>());
					if (actor["TagComponent"]) {
						loadedActor = mLevel->createActorWithGUID(actor["Actor"].as<uint64_t>(), actor["TagComponent"]["Tag"].as<std::string>());
					} // Dont load actor if no TagComponent, every actor should have a TagComponent, so if an actor has no TagComponent, it must be some kind of core thing

					if (actor["TransformComponent"]) {
						loadedActor.getComponent<Components::TransformComponent>().setTranslation(actor["TransformComponent"]["Translation"].as<glm::vec3>());
						loadedActor.getComponent<Components::TransformComponent>().setRotation(actor["TransformComponent"]["Rotation"].as<glm::vec3>());
						loadedActor.getComponent<Components::TransformComponent>().setScale(actor["TransformComponent"]["Scale"].as<glm::vec3>());

					}
					// AUDIO
					if (actor["AudioComponent"]) {
						loadedActor.addComponent<Components::AudioComponent>();

						loadedActor.getComponent<Components::AudioComponent>().file = actor["AudioComponent"]["File"].as<std::string>();
						loadedActor.getComponent<Components::AudioComponent>().properties.volume = actor["AudioComponent"]["Properties"]["Volume"].as<float>();
						loadedActor.getComponent<Components::AudioComponent>().properties.pitch = actor["AudioComponent"]["Properties"]["Pitch"].as<float>();
					}
					// CAMERA
					if (actor["CameraComponent"]) {
						loadedActor.addComponent<Components::CameraComponent>();

						loadedActor.getComponent<Components::CameraComponent>().setProjectionType((Components::CameraComponent::ProjectType)actor["CameraComponent"]["ProjectionType"].as<int>());
						loadedActor.getComponent<Components::CameraComponent>().setFOV(actor["CameraComponent"]["FOV"].as<float>());
						loadedActor.getComponent<Components::CameraComponent>().setNearClip(actor["CameraComponent"]["NearClipPlane"].as<float>());
						loadedActor.getComponent<Components::CameraComponent>().setFarClip(actor["CameraComponent"]["FarClipPlane"].as<float>());
						loadedActor.getComponent<Components::CameraComponent>().ar = actor["CameraComponent"]["AspectRatio"].as<float>();
					}
					if (actor["BillboardComponent"]) {
						AssetManager::emplaceTexture(
							actor["BillboardComponent"]["TexPath"].as<std::string>()						
						);
						loadedActor.addComponent<Components::BillboardComponent>(
							actor["BillboardComponent"]["TexPath"].as<std::string>());
					}
					if (actor["ScriptComponent"]) {
						loadedActor.addComponent<Components::ScriptComponent>();

						loadedActor.getComponent<Components::ScriptComponent>().name = actor["ScriptComponent"]["Module"].as<std::string>();
						loadedActor.getComponent<Components::ScriptComponent>().lang = actor["ScriptComponent"]["Language"].as<std::string>() == "C#"? 0 : 1;
					}

					if (actor["MeshComponent"]) {
						AssetManager::emplaceMesh(
							actor["MeshComponent"]["MeshPath"].as<std::string>(),
							(MeshType)actor["MeshComponent"]["MeshFormat"].as<int>()
						);
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
			mLevel->currentpath = sourcePath;
			return LevelMgrResults::SuccessResult;
		}

		LevelMgrResults LevelManager::loadRuntime(const std::string& sourcePath) {
			SHARD3D_ASSERT(false);
			return LevelMgrResults::ErrorResult;
		}
	}
}