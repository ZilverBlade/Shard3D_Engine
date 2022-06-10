#pragma once

#include "../../wb3d/actor.hpp"
#include "../../components.hpp"
#include "../../engine_window.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
namespace Shard3D {
	namespace controller {
		class EditorMouseMovementController {
		public:
			struct ButtonMappings {
				int canRotate = GLFW_MOUSE_BUTTON_RIGHT;
			};

			void moveInPlaneXZ(GLFWwindow* window, float dt, wb3d::Actor& actor);
			//void adjustFOV(GLFWwindow* window, glm::vec2 scrollPosition);

			ButtonMappings buttons{};
			float sensitivity{ 15.f };

		private:

			glm::vec3 orientation{ .0f, .0f, glm::radians(360.f)};
			glm::vec3 upVec{ 0.f, 1.f, 0.f };
			bool firstClick = true;
			//GLFWscrollfun scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		};
	}
}

