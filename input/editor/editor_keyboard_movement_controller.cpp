#include "../../s3dtpch.h"
#include "editor_keyboard_movement_controller.hpp"
#include <limits>

namespace Shard3D {
	namespace controller {
		void EditorKeyboardMovementController::moveInPlaneXY(GLFWwindow* window, float dt, wb3d::Actor& actor) {
			float yaw = actor.getComponent<Components::TransformComponent>().getRotation().z;
			float pitch = actor.getComponent<Components::TransformComponent>().getRotation().x;
			const glm::vec3 forwardDir{ sin(yaw), cos(yaw), -sin(pitch) };
			const glm::vec3 rightDir{ forwardDir.y, -forwardDir.x, 0.f, };
			const glm::vec3 upDir{ 0.f, 0.f, 1.f };

			glm::vec3 moveDir{ 0.f };
			
			if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
			if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
			if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
			if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
			if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
			if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

			speedModifier = 0.25f + 0.75f * (glfwGetKey(window, keys.slowDown) != GLFW_PRESS);

			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				actor.getComponent<Components::TransformComponent>().setTranslation(
					actor.getComponent<Components::TransformComponent>().getTranslation() += moveSpeed * dt * glm::normalize(moveDir) * speedModifier);
			}
		}
	}
}