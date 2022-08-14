#include "../../s3dpch.h"
#include "editor_movement_controller.h"
#include <limits>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Shard3D {
	namespace controller {
		void EditorMovementController::tryPoll(EngineWindow& _window, float _dt, ECS::Actor& _actor) {
			window = _window.getGLFWwindow();
			actor = _actor;
			dt = _dt;
			SHARD3D_EVENT_BIND_HANDLER_RFC(_window, EditorMovementController::eventEvent);

			float yaw = actor.getComponent<Components::TransformComponent>().getRotation().z;
			float pitch = actor.getComponent<Components::TransformComponent>().getRotation().x;
			const glm::vec3 forwardDir{ sin(yaw), cos(yaw), -sin(pitch) };
			const glm::vec3 rightDir{ forwardDir.y, -forwardDir.x, 0.f, };
			const glm::vec3 upDir{ 0.f, 0.f, 1.f };

			glm::vec3 moveDir{ 0.f };

			if (Input::isKeyDown(keys.moveForward)) moveDir += forwardDir;
			if (Input::isKeyDown(keys.moveBackward)) moveDir -= forwardDir;
			if (Input::isKeyDown(keys.moveRight)) moveDir += rightDir;
			if (Input::isKeyDown(keys.moveLeft)) moveDir -= rightDir;
			if (Input::isKeyDown(keys.moveUp)) moveDir += upDir;
			if (Input::isKeyDown(keys.moveDown)) moveDir -= upDir;

			speedModifier = 0.25f + 0.75f * (!Input::isKeyDown(keys.slowDown));

			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				actor.getComponent<Components::TransformComponent>().setTranslation(
					actor.getComponent<Components::TransformComponent>().getTranslation() += moveSpeed * dt * glm::normalize(moveDir) * speedModifier);
			}

			if (Input::isMouseButtonDown(buttons.canRotate)) {
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

				float rotX = sensitivity * (float)(mouseY - (height / 2.f)) / height;
				float rotY = sensitivity * (float)(mouseX - (width / 2.f)) / width;

				// up down rotation
				orientation = glm::rotate(orientation, glm::radians(rotX), upVec);
				// to make sure it doesnt over-rotate			
				orientation.x = glm::clamp(orientation.x, -1.57079632679f, 1.57079632679f);

				// left right rotation
				orientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, upVec)));

				// force the roll to be pi*2 radians
				orientation.z = 6.28318530718f;

				glfwSetCursorPos(window, (width / 2.f), (height / 2.f));
			}
			else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); firstClick = true; }

			if (glm::dot(orientation, orientation) > std::numeric_limits<float>::epsilon()) {
				actor.getComponent<Components::TransformComponent>().setRotation({ orientation.x, orientation.z, orientation.y });
			}
		}
		void EditorMovementController::eventEvent(Events::Event& e) {
			SHARD3D_EVENT_CREATE_DISPATCHER(e);
			
			//SHARD3D_EVENT_DISPATCH(KeyDownEvent, controller::EditorMovementController::keyDownEvent);
			//SHARD3D_EVENT_DISPATCH(MouseButtonDownEvent, controller::EditorMovementController::mouseButtonDownEvent);
			SHARD3D_EVENT_DISPATCH(Events::MouseScrollEvent, controller::EditorMovementController::mouseScrollEvent);
		}
		bool EditorMovementController::keyDownEvent(Events::KeyDownEvent& e) {
			float yaw = actor.getComponent<Components::TransformComponent>().getRotation().z;
			float pitch = actor.getComponent<Components::TransformComponent>().getRotation().x;
			const glm::vec3 forwardDir{ sin(yaw), cos(yaw), -sin(pitch) };
			const glm::vec3 rightDir{ forwardDir.y, -forwardDir.x, 0.f, };
			const glm::vec3 upDir{ 0.f, 0.f, 1.f };

			glm::vec3 moveDir{ 0.f };

			if (Input::isKeyDown(keys.moveForward)) moveDir += forwardDir;
			if (Input::isKeyDown(keys.moveBackward)) moveDir -= forwardDir;
			if (Input::isKeyDown(keys.moveRight)) moveDir += rightDir;
			if (Input::isKeyDown(keys.moveLeft)) moveDir -= rightDir;
			if (Input::isKeyDown(keys.moveUp)) moveDir += upDir;
			if (Input::isKeyDown(keys.moveDown)) moveDir -= upDir;

			speedModifier = 0.25f + 0.75f * (!Input::isKeyDown(keys.slowDown));

			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				actor.getComponent<Components::TransformComponent>().setTranslation(
					actor.getComponent<Components::TransformComponent>().getTranslation() += moveSpeed * dt * glm::normalize(moveDir) * speedModifier);
			}
			return false;
		}
		bool EditorMovementController::mouseButtonDownEvent(Events::MouseButtonDownEvent& e) {
			if (e.getButtonCode() == buttons.canRotate) {
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
			} else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); firstClick = true; }

			if (glm::dot(orientation, orientation) > std::numeric_limits<float>::epsilon()) {
				actor.getComponent<Components::TransformComponent>().setRotation({ orientation.x, orientation.z, orientation.y });
			}
			return false;
		}
		bool EditorMovementController::mouseScrollEvent(Events::MouseScrollEvent& e) {
			float fov = actor.getComponent<Components::CameraComponent>().getFOV();
			fov -= e.getYOffset() * 4;
			actor.getComponent<Components::CameraComponent>().setFOV(glm::clamp(fov, 30.f, 170.f));
			return false;
		}
	}
}