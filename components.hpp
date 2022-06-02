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

		struct Model3DComponent {
			std::shared_ptr<EngineModel> model{};
			std::string modelPath;

			Model3DComponent() = default;
			Model3DComponent(const Model3DComponent&) = default;
			Model3DComponent(const std::shared_ptr<EngineModel>& mdl) { model = mdl; modelPath = "modeldata/FART.obj"; }

			std::shared_ptr<EngineModel> getModel() { return model; };
		};

		struct PointlightComponent {
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			glm::vec4 attenuationMod = glm::vec4(0.f, 0.f, 1.f, 0.f);
			float specularMod = 1.0f;
		};

		struct SpotlightComponent {
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float outerAngle = glm::radians(15.0f);
			float innerAngle = glm::radians(45.0f);
			glm::vec4 attenuationMod = glm::vec4(0.f, 0.f, 1.f, 0.f);
			float specularMod = 1.0f;
		};

		struct DirectionalLightComponent {
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float specularMod = 1.0f;
		};
	}
}