#pragma once

#include "shard_model.hpp"
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

namespace shard {

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
	};

	class ShardGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, ShardGameObject>;

		static ShardGameObject createGameObject() {
			static id_t currentId = 0;
			return ShardGameObject{ currentId++ };
		}

		static ShardGameObject makePointlight(float intensity = 5.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f), glm::vec4 attenuationMod = glm::vec4(0.f, 0.f, 1.f, 0.f));

		ShardGameObject(const ShardGameObject&) = delete;
		ShardGameObject &operator=(const ShardGameObject&) = delete;
		ShardGameObject(ShardGameObject&&) = default;
		ShardGameObject &operator=(ShardGameObject&&) = default;

		id_t getId() { return id; }
		

		glm::vec3 color{};
		TransformComponent transform{};

		// Optional pointer components
		std::shared_ptr<ShardModel> model{};
		std::unique_ptr<PointlightComponent> pointlight = nullptr;

	private:
		ShardGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}