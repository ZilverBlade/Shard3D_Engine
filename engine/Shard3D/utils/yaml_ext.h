#pragma once
#define YAML_CPP_STATIC_DEFINE

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
#include "../core/asset/assetid.h"

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

	template<>
	struct convert<std::vector<Shard3D::AssetID>> {
		static Node encode(const std::vector<Shard3D::AssetID>& rhs) {
			Node node;
			for (auto& item : rhs) {
				node.push_back(item.getFile());
			}
			return node;
		}
		static bool decode(const Node& node, std::vector<Shard3D::AssetID>& rhs) {
			for (auto& item : node) {
				rhs.push_back(item.as<std::string>());
			}
			return true;
		}
	};
}

namespace Shard3D {
	// YAML spec used: https://yaml.org/spec/1.2.2/
	static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;	// [float, float]
	}
	static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out; // [float, float, float]
	}
	static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out; // [float, float, float, float]
	}
	static YAML::Emitter& operator<<(YAML::Emitter& out, const std::vector<AssetID>& a) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (auto& v : a)
			out << v.getFile();
		out << YAML::EndSeq;
		return out;
	}
}
