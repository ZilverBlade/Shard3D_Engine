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

		static void saveActor(YAML::Emitter& out, Actor actor, Level* level) {
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

			// RELATIONSHIP
			if (actor.hasComponent<Components::RelationshipComponent>()) {
				out << YAML::Key << "RelationshipComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "Parent" << YAML::Value << ((actor.getComponent<Components::RelationshipComponent>().parentActor != entt::null) ?  Actor(actor.getComponent<Components::RelationshipComponent>().parentActor, level).getUUID() : UUID(1));
				std::vector<uint64_t> correctedIDs;
				correctedIDs.reserve(actor.getComponent<Components::RelationshipComponent>().childActors.size());

				for (auto& child : actor.getComponent<Components::RelationshipComponent>().childActors)
					correctedIDs.push_back(Actor(child, level).getUUID());
				
				out << YAML::Key << "Children" << YAML::Value << correctedIDs;
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
				out << YAML::Key << "TextureAsset" << YAML::Value << actor.getComponent<Components::BillboardComponent>().asset.getFile();
				out << YAML::Key << "BillboardOrientation" << YAML::Value << (int)actor.getComponent<Components::BillboardComponent>().orientation;
				out << YAML::EndMap;
			}

			// MODEL
			if (actor.hasComponent<Components::Mesh3DComponent>()) {
				out << YAML::Key << "Mesh3DComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "MeshAsset" << YAML::Value << actor.getComponent<Components::Mesh3DComponent>().asset.getFile();
					out << YAML::Key << "Materials" << YAML::Value << actor.getComponent<Components::Mesh3DComponent>().materials;
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
			out << YAML::Key << "Level" << YAML::Value << AssetUtils::truncatePath(destinationPath);
			out << YAML::Key << "LastSavedEditorCameraPos" << YAML::Value;
			if (mLevel->actorMap.find(0) != mLevel->actorMap.end())
				out << mLevel->getActorFromUUID(0).getTransform().getTranslation();
			else
				out << glm::vec3(0.f);
			out << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;

			mLevel->registry.each([&](auto actorGUID) { Actor actor = { actorGUID, mLevel.get() };
				if (!actor) return;
				saveActor(out, actor, mLevel.get());
			});

			out << YAML::EndSeq;
			out << YAML::EndMap;
			std::string newPath = destinationPath;
			if (!strUtils::hasEnding(destinationPath, ".s3dlevel")) newPath = destinationPath + ".s3dlevel";
			
			std::ofstream fout(newPath);
			fout << out.c_str();
			fout.flush();
			fout.close();
		
			SHARD3D_LOG("Saved scene '{0}'", newPath);
			mLevel->currentpath = destinationPath;
		}

		void LevelManager::saveRuntime(const std::string& destinationPath) {
			SHARD3D_ASSERT(false);
		}

		LevelMgrResults LevelManager::load(const std::string& sourcePath, bool ignoreWarns) {
			std::ifstream stream(sourcePath);
			std::stringstream strStream;
			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());

			try {
				if (ignoreWarns == false) {
					if (!data["Level"]) return LevelMgrResults::WrongFileResult;

					if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
						SHARD3D_WARN("Incorrect engine version");
						return LevelMgrResults::OldEngineVersionResult;
					}// change this to check if the version is less or more
				}

				ResourceHandler::clearAllAssets(); // remove since new stuff will be loaded into memory
				
				std::string levelName = data["Level"].as<std::string>();
				mLevel->getActorFromUUID(0).getTransform().setTranslation(data["LastSavedEditorCameraPos"].as<glm::vec3>());

				if (data["Actors"]) {
					for (auto actor : data["Actors"]) {
						Actor loadedActor{};

						SHARD3D_INFO("Loading actor with ID {0}", actor["Actor"].as<uint64_t>());
						if (actor["TagComponent"]) {
							loadedActor = mLevel->createActorWithUUID(actor["Actor"].as<uint64_t>(), actor["TagComponent"]["Tag"].as<std::string>());
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
						}
						if (actor["BillboardComponent"]) {
							ResourceHandler::loadTexture(
								actor["BillboardComponent"]["TextureAsset"].as<std::string>()						
							);
							loadedActor.addComponent<Components::BillboardComponent>(
								actor["BillboardComponent"]["TextureAsset"].as<std::string>());
						}
						if (actor["ScriptComponent"]) {
							loadedActor.addComponent<Components::ScriptComponent>();

							loadedActor.getComponent<Components::ScriptComponent>().name = actor["ScriptComponent"]["Module"].as<std::string>();
							loadedActor.getComponent<Components::ScriptComponent>().lang = actor["ScriptComponent"]["Language"].as<std::string>() == "C#"? 0 : 1;
						}

						if (actor["Mesh3DComponent"]) {
							ResourceHandler::loadMesh(
								actor["Mesh3DComponent"]["MeshAsset"].as<std::string>()
							);
							loadedActor.addComponent<Components::Mesh3DComponent>(
								actor["Mesh3DComponent"]["MeshAsset"].as<std::string>());
							loadedActor.getComponent<Components::Mesh3DComponent>().materials = actor["Mesh3DComponent"]["Materials"].as<std::vector<AssetID>>();
							loadedActor.getComponent<Components::Mesh3DComponent>().validate();
							for (auto& material : loadedActor.getComponent<Components::Mesh3DComponent>().materials) {
								ResourceHandler::loadSurfaceMaterialRecursive(material);
							}
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
					for (auto actor : data["Actors"]) {
						
						if (actor["RelationshipComponent"]) {		
							Actor loadedActor{};
							loadedActor = mLevel->getActorFromUUID(actor["Actor"].as<uint64_t>());
							loadedActor.addComponent<Components::RelationshipComponent>();
							if (actor["RelationshipComponent"]["Parent"].as<uint64_t>() != 1)
								loadedActor.getComponent<Components::RelationshipComponent>().parentActor = mLevel->getActorFromUUID(actor["RelationshipComponent"]["Parent"].as<uint64_t>()).actorHandle;
							
							loadedActor.getComponent<Components::RelationshipComponent>().childActors.reserve(actor["RelationshipComponent"]["Children"].as<std::vector<uint64_t>>().size());

							for (auto& child : actor["RelationshipComponent"]["Children"].as<std::vector<uint64_t>>())
								loadedActor.getComponent<Components::RelationshipComponent>().childActors.push_back(mLevel->getActorFromUUID(child).actorHandle);
						}

					}
				}
			}
			catch (YAML::Exception ex) {
				SHARD3D_ERROR("Unable to load level {0}. Reason: {1}", sourcePath, ex.msg);
			}
			mLevel->name = AssetUtils::truncatePath(sourcePath);
			mLevel->currentpath = sourcePath;
			return LevelMgrResults::SuccessResult;
		}

		LevelMgrResults LevelManager::loadRuntime(const std::string& sourcePath) {
			SHARD3D_ASSERT(false);
			return LevelMgrResults::ErrorResult;
		}
	}
}