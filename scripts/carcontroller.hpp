#pragma once
#include "../wb3d/acting_actor.hpp"

namespace Shard3D {
	namespace CppScripts {
		class CarController : public wb3d::ActingActor {
		public:
			wb3d::Actor camAct;
			float exponent;
			glm::vec3 moveVector;
			void beginEvent() {}
			void endEvent() {}

			void tickEvent(float dt) {	
				float yaw = thisActor.getComponent<Components::TransformComponent>().rotation.y + glm::radians(-90.f);
				float pitch = thisActor.getComponent<Components::TransformComponent>().rotation.x;
				const glm::vec3 forwardDir{ sin(yaw), -sin(pitch), cos(yaw) };
				if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
					thisActor.getComponent<Components::TransformComponent>().rotation.y += 0.8f * dt;
				if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
					thisActor.getComponent<Components::TransformComponent>().rotation.y -= 0.8f * dt;
				if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
					moveVector -= dt * forwardDir;
				if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS) {
					moveVector += dt * exponent * forwardDir;
					exponent += 0.05f * dt;
				}
				else {
					moveVector.x -= dt * 0.01f;
					moveVector.z -= dt * 0.01f;
					exponent -= 0.05f * dt;
					if (moveVector.x < 0) moveVector.x = 0.f;
					if (moveVector.z < 0) moveVector.z = 0.f;
					if (exponent < 0) exponent = 0.f;
				}

				thisActor.getComponent<Components::TransformComponent>().translation += moveVector;
				camAct.getComponent<Components::TransformComponent>().translation = 
					glm::vec3(thisActor.getComponent<Components::TransformComponent>().translation.x,
						thisActor.getComponent<Components::TransformComponent>().translation.y + 1.5f,
						thisActor.getComponent<Components::TransformComponent>().translation.z - 4.f - glm::dot(moveVector, moveVector) * 5);
				camAct.getComponent<Components::CameraComponent>().fov = 60.f + glm::dot(moveVector, moveVector) * 10;
			}

			void spawnEvent() {
				camAct = getActiveLevel()->createActor("cambeam");
				camAct.addComponent<Components::CameraComponent>();
				posessCameraActor(camAct);
				camAct.getComponent<Components::TransformComponent>().rotation.x = 0.3f;

				for (int i = 0; i < 10; i++) {
					auto light = getActiveLevel()->createActor("pointlight" + i);
					light.addComponent<Components::PointlightComponent>();
					light.getComponent<Components::TransformComponent>().translation.y = 3.f;
					light.getComponent<Components::TransformComponent>().translation.z = 25.f + i * 3.f;
				}
				
			}

			void killEvent() {
			}
		};
	}
}