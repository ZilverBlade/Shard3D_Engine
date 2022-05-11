#pragma once

#include "shard_model.hpp"
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace shard {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f};
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};
	class ShardGameObject {
	public:
		using id_t = unsigned int;

		static ShardGameObject createGameObject() {
			static id_t currentId = 0;
			return ShardGameObject{ currentId++ };
		}

		ShardGameObject(const ShardGameObject&) = delete;
		ShardGameObject &operator=(const ShardGameObject&) = delete;
		ShardGameObject(ShardGameObject&&) = default;
		ShardGameObject &operator=(ShardGameObject&&) = default;

		id_t getId() { return id; }

		std::shared_ptr<ShardModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		ShardGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}