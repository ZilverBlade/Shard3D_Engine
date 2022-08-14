#pragma once
#include <Shard3D/core/ecs/sactor.h>
#include <chrono>
namespace Shard3D::CppScripts {
	class CarController : public ECS::SActor {
		const float basePitch = 0.1f;
	public:
		ECS::Actor camAct;
		float exponent;
		glm::vec3 moveVector;
		void beginEvent() override {
			thisActor.getComponent<Components::TransformComponent>().setTranslation({ 0.f, 0.f,  0.2f });
			camAct = getActiveLevel()->createActor("cambeam");
			camAct.addComponent<Components::CameraComponent>();

			camAct.getComponent<Components::TransformComponent>().setRotation({ 0.3f, 0.f, 0.f });

			for (int i = 0; i < 17; i++) {
				auto light = getActiveLevel()->createActor("pointlight" + std::to_string(i));
				light.addComponent<Components::PointlightComponent>().lightIntensity = 3.f;
				light.getComponent<Components::PointlightComponent>().color = { glm::clamp(i * 0.1f, 0.f, 3.f), glm::clamp(i * 0.15f, 0.f, 3.f), glm::clamp(i * 0.2f, 0.f, 3.f) };
				light.getComponent<Components::TransformComponent>().setTranslation({ 5.f,
																					25.f + i * 15.f,
																					 4.1f});
			}
			for (int i = 0; i < 17; i++) {
				auto light = getActiveLevel()->createActor("spotlight" + std::to_string(i));
				light.addComponent<Components::SpotlightComponent>().lightIntensity = 6.f;
				light.getComponent<Components::SpotlightComponent>().color = { glm::clamp(i * 0.1f, 0.f, 3.f), glm::clamp(i * 0.15f, 0.f, 3.f), glm::clamp(i * 0.2f, 0.f, 3.f) };
				light.getComponent<Components::TransformComponent>().setRotationZ(-0.1f);

				light.getComponent<Components::TransformComponent>().setTranslation({ 5.f,
																					25.f + 17.f * 15.f + i * 15.f,
																					4.1f });
			}

			thisActor.getComponent<Components::AudioComponent>().play();
			thisActor.getComponent<Components::AudioComponent>().properties.pitch = basePitch;
			thisActor.getComponent<Components::AudioComponent>().properties.volume = 2.f;

			//camAct.getComponent<Components::CameraComponent>().fov = 60.f;
			camAct.getComponent<Components::CameraComponent>().camera.setViewYXZ(
				camAct.getComponent<Components::TransformComponent>().getTranslation(),
				camAct.getComponent<Components::TransformComponent>().getRotation()
			);
			possessCameraActor(camAct);
			moveVector = glm::vec3(0.001f);
		}
		void endEvent() override {
			thisActor.getComponent<Components::AudioComponent>().stop();
		}

		void tickEvent(float dt) override {
			auto rotation = thisActor.getComponent<Components::TransformComponent>().getRotation();
			auto translation = thisActor.getComponent<Components::TransformComponent>().getTranslation();

			float yaw = thisActor.getComponent<Components::TransformComponent>().getRotation().z + glm::radians(-90.f);
			const glm::vec3 forwardDir{ sin(yaw), cos(yaw), -sin(0) };
			
			if (Input::isKeyDown(GLFW_KEY_D))
				rotation.z += 0.8f * dt;
			if (Input::isKeyDown(GLFW_KEY_A))
				rotation.z -= 0.8f * dt;
			if (Input::isKeyDown(GLFW_KEY_S))
				moveVector -= dt;
			if (Input::isKeyDown(GLFW_KEY_W)) {
				moveVector += exponent * 0.008f;
				exponent += dt * 0.005f * glm::clamp(0.1f / moveVector.z, 0.f, 10.f);
				thisActor.getComponent<Components::AudioComponent>().properties.volume = 5.f;
			} else {
				moveVector.x -= dt * 0.0098f;
				moveVector.y -= dt * 0.0098f;
				exponent -= 0.05f * dt;
				if (exponent < 0) exponent = 0.0001f;
				if (moveVector.x < 0) moveVector.x = 0.f;
				if (moveVector.y < 0) moveVector.y = 0.f;
				thisActor.getComponent<Components::AudioComponent>().properties.volume = 3.f;
			}
			translation += moveVector * forwardDir;

			updateCameraPos();
			thisActor.getComponent<Components::AudioComponent>().properties.relativePos = translation;
			thisActor.getComponent<Components::AudioComponent>().properties.pitch = basePitch + moveVector.y * 3.f;
			thisActor.getComponent<Components::AudioComponent>().update();

			thisActor.getComponent<Components::TransformComponent>().setRotation(rotation);
			thisActor.getComponent<Components::TransformComponent>().setTranslation(translation);
		}

		void updateCameraPos() {
			glm::vec3 cameraPos = thisActor.getTransform().mat4() * glm::vec4(4.f, 0.f, 0.f, 1.f);

			camAct.getComponent<Components::TransformComponent>().setTranslation(glm::vec3(cameraPos.x, cameraPos.z, cameraPos.y + 1.5f));
			camAct.getComponent<Components::TransformComponent>().setRotationZ(thisActor.getTransform().getRotation().z - 1.57079632679f);

			camAct.getComponent<Components::CameraComponent>().setFOV(
				glm::clamp(60.f + glm::dot(moveVector, moveVector) *
					glm::max(1.0f, (5.f / glm::dot(moveVector, moveVector))) * 10.f, 60.f, 90.f));
		}

		void spawnEvent() override {
			
		}

		void killEvent() override {
			
		}
	};
}