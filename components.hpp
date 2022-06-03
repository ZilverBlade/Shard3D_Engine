#pragma once

#include "model.hpp"
#include "GUID.hpp"

#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

namespace Shard3D {
	namespace Components {
		struct GUIDComponent {
			GUID id;

			GUIDComponent() = default;
			GUIDComponent(const GUIDComponent&) = default;
		};

		struct TagComponent {
			std::string tag;

			TagComponent() = default;
			TagComponent(const TagComponent&) = default;
			operator std::string() { return tag; };
		};

		struct TransformComponent {
			glm::vec3 translation{ 0.f, 0.f, 0.f };
			glm::vec3 scale{ 1.f, 1.f, 1.f };
			glm::vec3 rotation{ 0.f, 0.f, 0.f };

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;

			glm::mat4 mat4();
			glm::mat3 normalMatrix();
		};

		struct MeshComponent {
			std::shared_ptr<EngineModel> model{};
			std::string path{};
			ModelType type = ModelType::MODEL_TYPE_NULL;

			MeshComponent() = default;
			MeshComponent(const MeshComponent&) = default;
			MeshComponent(const std::shared_ptr<EngineModel>& mdl) { model = mdl; }

			std::shared_ptr<EngineModel> getModel() { return model; };
			std::string getPath() { return "modeldata/axis.obj"; };
			ModelType getModelType() { return type; };
			/*
			std::string operator()(ModelType) {
				if (type == ModelType::MODEL_TYPE_NULL) return "null";
				if (type == ModelType::MODEL_TYPE_OBJ) return "obj_type";
				if (type == ModelType::MODEL_TYPE_COLLADA) return "collada_type";
				if (type == ModelType::MODEL_TYPE_GLTF) return "gltf_type";
				if (type == ModelType::MODEL_TYPE_FBX) return "fbx_type";
			}
			*/
		};

		struct PointlightComponent {
			float radius = 1.f;
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			glm::vec3 attenuationMod = { 0.f, 0.f, 1.f };
			float specularMod = 1.0f;
		};

		struct SpotlightComponent {
			float radius = 1.f;
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float outerAngle = glm::radians(15.0f);
			float innerAngle = glm::radians(45.0f);
			glm::vec3 attenuationMod = { 0.f, 0.f, 1.f };
			float specularMod = 1.0f;
		};

		struct DirectionalLightComponent {
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float specularMod = 1.0f;
		};
	}
}