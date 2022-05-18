#include "mouse_movement_controller.hpp"
#include <limits>
#include <iostream>
namespace Shard3D {
	namespace controller {

		bool firstMouse = true;

		float lastX, lastY;

		void MouseMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, EngineGameObject& gameObject, glm::vec2 mousePosition) {
			glm::vec3 rotate{ 0 };
			if (glfwGetMouseButton(window, buttons.canRotate) == GLFW_PRESS) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);		

				rotate.y += (mousePosition.x - lastX);
				rotate.x += (lastY - mousePosition.y);

				lastX = mousePosition.x;
				lastY = mousePosition.y;
			}
			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.rotation += sensitivity * dt * glm::normalize(rotate);
			}
		}
		/**/
		void MouseMovementController::adjustFOV(GLFWwindow* window, glm::vec2 scrollPosition) {
			//glfwSetScrollCallback(window, scroll_callback);

			float fov = {};
			if (fov < 30.0f)
				fov = 30.0f;
			if (fov > 120.0f)
				fov = 120.0f;

			fov -= scrollPosition.y;

			//return fov;
		}

		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
		{
		}
	}
}