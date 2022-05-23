#include "mouse_movement_controller.hpp"
#include <limits>
#include <iostream>

namespace Shard3D {
	namespace controller {

		bool firstClick = true;
		float lastX, lastY;

		void MouseMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, EngineGameObject& gameObject, glm::vec2 mousePosition) {

			glm::vec3 rotate{ 0 };
			if (glfwGetMouseButton(window, buttons.canRotate) == GLFW_PRESS) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				// Prevents camera from jumping on the first click
				if (firstClick)
				{
					lastX = mousePosition.x;
					lastY = mousePosition.y;
					firstClick = false;
					return;					
				}

				rotate.y += (mousePosition.x - lastX);
				rotate.x += (lastY - mousePosition.y);

				lastX = mousePosition.x;
				lastY = mousePosition.y;
			}
			else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); firstClick = true; }
			
			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) 
				gameObject.transform.rotation += sensitivity * dt * rotate;
		}
		/*
		void MouseMovementController::adjustFOV(GLFWwindow* window, glm::vec2 scrollPosition) {
			double xOffset{};
			double yOffset{};
			glfwSetScrollCallback(window, scroll_callback(window, xOffset, yOffset));

		}

		GLFWscrollfun scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

			float fov = {};
			if (fov < 30.0f)
				fov = 30.0f;
			if (fov > 120.0f)
				fov = 120.0f;

			fov -= yoffset;
			std::cout << fov << "\n";
			;
		}
		*/
	}
}