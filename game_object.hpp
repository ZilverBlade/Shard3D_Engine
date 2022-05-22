#pragma once

#include "model.hpp"
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

namespace Shard3D {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f};
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointlightComponent {
		float lightIntensity = 1.0f;
		glm::vec4 attenuationMod = glm::vec4(0.f, 0.f, 1.f, 0.f);
		float specularMod = 1.0f;
	};

	struct SpotlightComponent {
		float lightIntensity = 1.0f;
		float outerAngle = glm::radians(45.0f);
		float innerAngle = glm::radians(15.0f);
		glm::vec4 attenuationMod = glm::vec4(0.f, 0.f, 1.f, 0.f);
		float specularMod = 1.0f;
	};

	struct DirectionalLightComponent {
		float lightIntensity = 1.0f;
		float specularMod = 1.0f;
	};

	class EngineGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, EngineGameObject>;

		static EngineGameObject createGameObject() {
			static id_t currentId = 0;
			return EngineGameObject{ currentId++ };
		}

		//LIGHT TYPES!
		static EngineGameObject makePointlight(
			float intensity = 5.f,
			float radius = 0.1f,
			glm::vec3 color = glm::vec3(1.f),
			glm::vec3 attenuationMod = glm::vec3(0.f, 0.f, 1.f),
			float specularMod = 1.0f
		);

		static EngineGameObject makeSpotlight(
			float intensity = 5.f,
			float radius = 0.1f,
			glm::vec3 color = glm::vec3(1.f),
			glm::vec3 direction = glm::vec3(1.f, -3.f, -1.f),
			float outerAngle = glm::radians(25.f), 
			float innerAngle = glm::radians(45.f), 
			glm::vec3 attenuationMod = glm::vec3(0.f, 0.f, 1.f),
			float specularMod = 1.0f
		);
		
		static EngineGameObject makeDirectionalLight(
			float intensity = 5.f, 
			glm::vec3 color = glm::vec3(1.f), 
			glm::vec3 direction = glm::vec3(1.f, -3.f, -1.f),
			float specularMod = 1.0f
		);

		EngineGameObject(const EngineGameObject&) = delete;
		EngineGameObject &operator=(const EngineGameObject&) = delete;
		EngineGameObject(EngineGameObject&&) = default;
		EngineGameObject &operator=(EngineGameObject&&) = default;

		id_t getId() { return id; }
		

		glm::vec3 color{};
		TransformComponent transform{};

		// Optional pointer components
		std::shared_ptr<EngineModel> model{};
		std::unique_ptr<PointlightComponent> pointlight = nullptr;
		std::unique_ptr<SpotlightComponent> spotlight = nullptr;
		std::unique_ptr<DirectionalLightComponent> directionalLight = nullptr;

	private:
		EngineGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}