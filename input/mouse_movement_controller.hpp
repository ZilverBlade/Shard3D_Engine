#pragma once

#include "../game_object.hpp"
#include "../engine_window.hpp"

namespace Shard3D {
	namespace controller {
		class MouseMovementController {
		public:
			struct ButtonMappings {
				int canRotate = GLFW_MOUSE_BUTTON_RIGHT;
			};

			void moveInPlaneXZ(GLFWwindow* window, float dt, EngineGameObject& gameObject, glm::vec2 mousePosition);
			void adjustFOV(GLFWwindow* window, glm::vec2 scrollPosition);

			ButtonMappings buttons{};
			float sensitivity{ 2.f };

		private:
			void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		};
	}
}

