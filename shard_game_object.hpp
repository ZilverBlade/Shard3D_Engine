#pragma once

#include "shard_model.hpp"
#include <memory>

namespace shard {

	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;
		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
			return rotMatrix * scaleMat;
		}
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
		Transform2dComponent transform2d;

	private:
		ShardGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}