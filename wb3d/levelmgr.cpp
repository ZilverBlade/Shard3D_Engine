#include "levelmgr.hpp"

#include "actor.hpp" 
#include "../components.hpp"

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
			if (!node.IsSequence() || node.size() != 2) return false;
			
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
			if (!node.IsSequence() || node.size() != 3) return false;

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
			if (!node.IsSequence() || node.size() != 4) return false;

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

		LevelManager::LevelManager(const std::shared_ptr<Level>& level) : mLevel(level) { }

		static void saveActor(YAML::Emitter& out, Actor actor) {
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
					out << YAML::Key << "MeshPath" << YAML::Value << actor.getComponent<Components::MeshComponent>().getPath();
					out << YAML::Key << "MeshFormat" << YAML::Value << (int)actor.getComponent<Components::MeshComponent>().getModelType();
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

		void LevelManager::save(const std::string& destinationPath)  {
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

			std::ofstream fout(destinationPath);
			fout << out.c_str();
		}

		void LevelManager::saveRuntime(const std::string& destinationPath) {
			assert(false);
		}

		LevelMgrResults LevelManager::load(const std::string& sourcePath) {
			std::ifstream stream(sourcePath);
			std::stringstream strStream;
			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());
			if (!data["Level"]) return LevelMgrResults::WrongFileResult;

			std::string levelName = data["Scene"].as<std::string>();
			
			if (data["Actors"]) {
				for (auto actor : data["Actors"]) {
					uint64_t guid = actor["Actor"].as<uint64_t>();

					Actor loadedActor{};
					if (actor["TagComponent"]) {
						Actor loadedActor = mLevel->createActorWithGUID(guid, actor["TagComponent"]["Tag"].as<std::string>());
					} // Dont load actor if no TagComponent, every actor should have a TagComponent, so if an actor has no TagComponent, it must be some kind of core thing

					if (actor["TransformComponent"]) {
#ifndef ACTOR_FORCE_TRANSFORM_COMPONENT		
						if (!loadedActor.hasComponent<Components::TransformComponent>()) loadedActor.addComponent<Components::TransformComponent>();
#endif
						loadedActor.getComponent<Components::TransformComponent>().translation = actor["TransformComponent"]["Translation"].as<glm::vec3>();
						loadedActor.getComponent<Components::TransformComponent>().rotation = actor["TransformComponent"]["Rotation"].as<glm::vec3>();
						loadedActor.getComponent<Components::TransformComponent>().scale = actor["TransformComponent"]["Scale"].as<glm::vec3>();
					}

				}
			}
		}

		LevelMgrResults LevelManager::loadRuntime(const std::string& sourcePath) {
			assert(false);
			return LevelMgrResults::ErrorResult;
		}
	}
}