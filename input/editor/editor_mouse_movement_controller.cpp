#include "../../s3dtpch.h"
#include "editor_mouse_movement_controller.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../../singleton.hpp"

namespace Shard3D {
	namespace controller {
		EditorMouseMovementController::EditorMouseMovementController() {
			glfwSetScrollCallback(Singleton::engineWindow.getGLFWwindow(), scroll_callback);
		}
		void EditorMouseMovementController::moveInPlaneXY(GLFWwindow* window, float dt, wb3d::Actor& actor) {
			adjustFOV(window, dt, actor);
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

				float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
				float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

				// up down rotation
				orientation = glm::rotate(orientation, glm::radians(rotX), upVec);
				// to make sure it doesnt over-rotate			
				orientation.x = glm::clamp(orientation.x, -1.57079632679f, 1.57079632679f);

				// left right rotation
				orientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, upVec)));
	
				// force the roll to be pi*2 radians
				orientation.z = 6.28318530718f;
				
				glfwSetCursorPos(window, (width / 2), (height / 2));
			}
			else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); firstClick = true; }

			if (glm::dot(orientation, orientation) > std::numeric_limits<float>::epsilon()) {
				actor.getComponent<Components::TransformComponent>().setRotation({ orientation.x, orientation.z, orientation.y });
			}
		}

		void EditorMouseMovementController::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
			float fov = cachedActor.getComponent<Components::CameraComponent>().getFOV();
			fov -= yoffset * 4;
			cachedActor.getComponent<Components::CameraComponent>().setFOV(glm::clamp(fov, 30.f, 170.f));
		}
		
		void EditorMouseMovementController::adjustFOV(GLFWwindow* window, float dt, wb3d::Actor& actor) {
			cachedActor = actor;
			actor = cachedActor;
		}

	}
}