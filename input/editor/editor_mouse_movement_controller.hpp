#pragma once

#include "../../wb3d/actor.hpp"
#include "../../components.hpp"
#include "../../engine_window.hpp"

namespace Shard3D {
	namespace controller {
		class EditorMouseMovementController {
		public:
			struct ButtonMappings {
				int canRotate = GLFW_MOUSE_BUTTON_RIGHT;
			};
			EditorMouseMovementController();
			void moveInPlaneXZ(GLFWwindow* window, float dt, wb3d::Actor& actor);

			ButtonMappings buttons{};
			float sensitivity{ 15.f };

		private:

			glm::vec3 orientation{ .0f, .0f, 6.28318530718f};
			glm::vec3 upVec{ 0.f, 1.f, 0.f };
			bool firstClick = true;
			static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
			static void adjustFOV(GLFWwindow* window, float dt, wb3d::Actor& actor);

			inline static wb3d::Actor cachedActor;

		};
	}
}

