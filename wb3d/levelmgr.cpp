#include "levelmgr.hpp"

#include "actor.hpp" 
#include "../components.hpp"
#include "../wb3d/wb3d_imgui_frame.cpp"
#include "../utils/definitions.hpp"


#define YAML_CPP_STATIC_DEFINE

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace YAML {
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}
		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() < 2) return false;
			
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() < 3) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() < 4) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Shard3D {
	namespace wb3d {

		// YAML spec used: https://yaml.org/spec/1.2.2/

		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
			out << YAML::Flow;
			out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
			return out;	// [float, float]
		}
		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
			out << YAML::Flow;
			out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
			return out; // [float, float, float]
		}
		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
			out << YAML::Flow;
			out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
			return out; // [float, float, float, float]
		}

		LevelManager::LevelManager(const std::shared_ptr<Level>& level) : mLevel(level) { 
			Log log;
			log.logString("Loading Level Manager");
		}

		std::string LevelManager::encrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string encryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				encryptedString.push_back((char)
					((((c + LEVEL_CIPHER_KEY) * 2) - LEVEL_CIPHER_KEY) / 2));
			}

			std::cout << "Duration of Level encryption: " << std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000 << "ms\n";
			return encryptedString;
		}
		std::string LevelManager::decrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string decryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				decryptedString.push_back((char)
					(((c * 2) + LEVEL_CIPHER_KEY) / 2) - LEVEL_CIPHER_KEY);
			}

			std::cout << "Duration of Level decryption: " << std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000 << "ms\n";
			return decryptedString;
		}

		static void saveActor(YAML::Emitter& out, Actor actor) {
			if (!actor.hasComponent<Components::GUIDComponent>()) return;
			if (actor.getGUID() == 0 || actor.getGUID() == sizeof(uint64_t)) return; // might be reserved for core engine purposes

			out << YAML::BeginMap;
			out << YAML::Key << "Actor" << YAML::Value << actor.getGUID();

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
					out << YAML::Key << "Translation" << YAML::Value << actor.getComponent<Components::TransformComponent>().translation;
					out << YAML::Key << "Rotation" << YAML::Value << actor.getComponent<Components::TransformComponent>().rotation;
					out << YAML::Key << "Scale" << YAML::Value << actor.getComponent<Components::TransformComponent>().scale;
				out << YAML::EndMap;
			}

			// MODEL
			if (actor.hasComponent<Components::MeshComponent>()) {
				out << YAML::Key << "MeshComponent";
				out << YAML::BeginMap;
					out << YAML::Key << "MeshPath" << YAML::Value << actor.getComponent<Components::MeshComponent>().file;
					out << YAML::Key << "MeshFormat" << YAML::Value << (int)actor.getComponent<Components::MeshComponent>().type;
					out << YAML::Key << "Indexed" << YAML::Value << actor.getComponent<Components::MeshComponent>().isIndexed;
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
			out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION;
			out << YAML::Key << "WorldBuilder3D" << YAML::Value << EDITOR_VERSION;
			out << YAML::Key << "Level" << YAML::Value << "Some kind of level";
			out << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;

			mLevel->eRegistry.each([&](auto actorGUID) { Actor actor = { actorGUID, mLevel.get() };
				if (!actor) return;
				saveActor(out, actor);
			});

			out << YAML::EndSeq;
			out << YAML::EndMap;

			if (encryptLevel) {
				std::ofstream fout(destinationPath);
				fout << encrypt(out.c_str());
				fout.flush();
				fout.close();
			} else {
				std::ofstream fout2(destinationPath);
				fout2 << out.c_str();
				fout2.flush();
				fout2.close();
			}
		
			std::cout << "Saved scene '" << destinationPath << "'";
		}

		void LevelManager::saveRuntime(const std::string& destinationPath) {
			assert(false);
		}

		LevelMgrResults LevelManager::load(const std::string& sourcePath, EngineDevice& device, bool ignoreWarns) {
			//mLevel->killEverything();
			std::ifstream stream(sourcePath);
			std::stringstream strStream;
			strStream << stream.rdbuf();

#if GAME_RELEASE_READY
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
				if (!data["Level"]) return LevelMgrResults::WrongFileResult;

				if (data["Shard3D"].as<std::string>() != ENGINE_VERSION) { 
					std::cout << "wrong engine version\n";
					return LevelMgrResults::OldEngineVersionResult;
				}// change this to check if the version is less or more
				if (data["WorldBuilder3D"].as<std::string>() != EDITOR_VERSION) {
					std::cout << "wrong editor version\n";
					return LevelMgrResults::OldEditorVersionResult;
				}// change this to check if the version is less or more

			}

			std::string levelName = data["Level"].as<std::string>();
			
			if (data["Actors"]) {
				for (auto actor : data["Actors"]) {
					Actor loadedActor{};

					std::cout << "Loading actor with ID " << actor["Actor"].as<uint64_t>() << "\n";
					if (actor["TagComponent"]) {
						loadedActor = mLevel->createActorWithGUID(actor["Actor"].as<uint64_t>(), actor["TagComponent"]["Tag"].as<std::string>());
					} // Dont load actor if no TagComponent, every actor should have a TagComponent, so if an actor has no TagComponent, it must be some kind of core thing

					if (actor["TransformComponent"]) {
#ifndef ACTOR_FORCE_TRANSFORM_COMPONENT		
						loadedActor.addComponent<Components::TransformComponent>();
#endif
						loadedActor.getComponent<Components::TransformComponent>().translation = actor["TransformComponent"]["Translation"].as<glm::vec3>();
						loadedActor.getComponent<Components::TransformComponent>().rotation = actor["TransformComponent"]["Rotation"].as<glm::vec3>();
						loadedActor.getComponent<Components::TransformComponent>().scale = actor["TransformComponent"]["Scale"].as<glm::vec3>();

					}

					if (actor["MeshComponent"]) {
						std::shared_ptr<EngineModel> model = EngineModel::createModelFromFile(
							device,
							actor["MeshComponent"]["MeshPath"].as<std::string>(),
				(ModelType) actor["MeshComponent"]["MeshFormat"].as<int>(),
							actor["MeshComponent"]["Indexed"].as<bool>()
						);
						loadedActor.addComponent<Components::MeshComponent>(model);
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
			return LevelMgrResults::SuccessResult;
		}

		LevelMgrResults LevelManager::loadRuntime(const std::string& sourcePath) {
			assert(false);
			return LevelMgrResults::ErrorResult;
		}
	}
}