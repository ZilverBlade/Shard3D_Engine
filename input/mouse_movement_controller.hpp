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

			void moveInPlaneXZ(GLFWwindow* window, float dt, EngineGameObject& gameObject);
			//void adjustFOV(GLFWwindow* window, glm::vec2 scrollPosition);

			ButtonMappings buttons{};
			float sensitivity{ 15.f };

		private:

			glm::vec3 orientation{ 0.f, 0.f, glm::radians(360.f) };;
			bool firstClick = true;
			//GLFWscrollfun scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		};
	}
}

