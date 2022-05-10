#pragma once

#include "../shard_game_object.hpp"
#include "../shard_window.hpp"

namespace shard {
	namespace controller {
		class MouseMovementController {
		public:
			struct ButtonMappings {
				int canRotate = GLFW_MOUSE_BUTTON_RIGHT;
			};

			void moveInPlaneXZ(GLFWwindow* window, float dt, ShardGameObject& gameObject, glm::vec2 mousePosition);
			void adjustFOV(GLFWwindow* window, glm::vec2 scrollPosition);

			ButtonMappings buttons{};
			float sensitivity{ 3.f };

		private:
			void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		};
	}
}

