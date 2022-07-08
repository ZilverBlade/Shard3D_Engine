#pragma once
#include "../wb3d/acting_actor.hpp"
#include <chrono>
namespace Shard3D::CppScripts {
	class CarController : public wb3d::ActingActor {
	public:
		wb3d::Actor camAct;
		float exponent;
		glm::vec3 moveVector;
		void beginEvent() override {}
		void endEvent() override {}

		void tickEvent(float dt) override {
			float yaw = thisActor.getComponent<Components::TransformComponent>().rotation.y + glm::radians(-90.f);
			const glm::vec3 forwardDir{ sin(yaw), -sin(0), cos(yaw) };
			if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
				thisActor.getComponent<Components::TransformComponent>().rotation.y += 0.8f * dt;
			if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
				thisActor.getComponent<Components::TransformComponent>().rotation.y -= 0.8f * dt;
			if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
				moveVector -= dt;
			if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS) {
				moveVector += dt * exponent;
				exponent += 0.005f * dt * glm::clamp(0.1f / moveVector.z, 0.f, 10.f);
			}
			else {
				moveVector.x -= dt * 0.0098f;
				moveVector.z -= dt * 0.0098f;
				exponent -= 0.05f * dt;
				if (moveVector.x < 0) moveVector.x = 0.f;
				if (moveVector.z < 0) moveVector.z = 0.f;
				if (exponent < 0) exponent = 0.f;
			}
			thisActor.getComponent<Components::TransformComponent>().translation += moveVector * forwardDir;
			camAct.getComponent<Components::TransformComponent>().translation =
				glm::vec3(thisActor.getComponent<Components::TransformComponent>().translation.x,
					thisActor.getComponent<Components::TransformComponent>().translation.y + 1.5f,
					glm::clamp(thisActor.getComponent<Components::TransformComponent>().
						translation.z - 4.f - moveVector.z * 5, 
						thisActor.getComponent<Components::TransformComponent>().translation.z - 15.f, 
						thisActor.getComponent<Components::TransformComponent>().translation.z  -4.f));

			camAct.getComponent<Components::CameraComponent>().fov = 
				glm::clamp(60.f + glm::dot(moveVector, moveVector) * 
					glm::max(1.0f, (1.f / glm::dot(moveVector, moveVector))) * 10, 60.f, 90.f);
		}

		void spawnEvent() override {
			thisActor.getComponent<Components::TransformComponent>().translation = { 0.f, 0.2f, 0.f };
			camAct = getActiveLevel()->createActor("cambeam");
			camAct.addComponent<Components::CameraComponent>();
			posessCameraActor(camAct);
			camAct.getComponent<Components::TransformComponent>().rotation.x = 0.3f;
			camAct.getComponent<Components::CameraComponent>().fov = 60.f;
			for (int i = 0; i < 17; i++) {
				auto light = getActiveLevel()->createActor("pointlight" + i);
				light.addComponent<Components::PointlightComponent>().lightIntensity = 3.f;
				light.getComponent<Components::PointlightComponent>().color = { glm::clamp(i * 0.1f, 0.f, 3.f), glm::clamp(i * 0.15f, 0.f, 3.f), glm::clamp(i * 0.2f, 0.f, 3.f) };
				light.getComponent<Components::TransformComponent>().translation = { 5.f,
																					4.1f,
																					25.f + i * 15.f };
				
			}
			for (int i = 0; i < 17; i++) {
				auto light = getActiveLevel()->createActor("spotlight" + i);
				light.addComponent<Components::SpotlightComponent>().lightIntensity = 6.f;
				light.getComponent<Components::SpotlightComponent>().color = { glm::clamp(i * 0.1f, 0.f, 3.f), glm::clamp(i * 0.15f, 0.f, 3.f), glm::clamp(i * 0.2f, 0.f, 3.f) };
				light.getComponent<Components::TransformComponent>().rotation.z = glm::radians(90.f);

				light.getComponent<Components::TransformComponent>().translation = { 5.f,
																					4.1f,
																					25.f + 17.f*15.f + i * 15.f };
			}
		}

		void killEvent() override {
		}
	};
}