#include "mouse_movement_controller.hpp"
#include <limits>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Shard3D {
	namespace controller {

		void MouseMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, EngineGameObject& gameObject) {

			
			if (glfwGetMouseButton(window, buttons.canRotate) == GLFW_PRESS) {
		
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				int width;
				int height;
				glfwGetWindowSize(window, &width, &height);

				// Prevents camera from jumping on the first click
				if (firstClick) {
					glfwSetCursorPos(window, (width / 2), (height / 2));
					firstClick = false;
				}

				double mouseX;
				double mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);

				// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
				// and then "transforms" them into degrees
				float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
				float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

				// Calculates upcoming vertical change in the Orientation
				glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, glm::vec3(0.f, 1.f, 0.f))));

				// Decides whether or not the next vertical Orientation is legal or not
				//if (abs(glm::angle(newOrientation, glm::vec3(0.f, -1.f, 0.f)) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
					orientation = newOrientation;
				//}

				// Rotates the Orientation left and right
				orientation = glm::rotate(orientation, glm::radians(-rotX), glm::vec3(0.f, 1.f, 0.f));

				std::cout << orientation.x << orientation.y << orientation.z << "\n";
				glfwSetCursorPos(window, (width / 2), (height / 2));
			}
			else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); firstClick = true; }

			if (glm::dot(orientation, orientation) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.rotation = { orientation.x, orientation.y, glm::radians(360.f) };
			}
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