#pragma once
#include "../wb3d/acting_actor.hpp"

namespace Shard3D {
	namespace CppScripts {
		class ExampleCppScript : public wb3d::ActingActor {
		public:
			wb3d::Actor camAct;
			void beginEvent() {
			
			}
			void endEvent()	{}

			void tickEvent(float dt) {	
				if (glfwGetKey(EngineWindow::getGLFWwindow(), GLFW_KEY_B) != GLFW_PRESS) {
					getComponent<Components::TransformComponent>().translation += dt;
				}

				camAct.getComponent<Components::TransformComponent>().translation += 1.f * dt;
			}

			void spawnEvent() {
				std::cout << "Example script!\n";
				camAct = getActiveLevel()->createActor("cambeam");
				camAct.addComponent<Components::CameraComponent>();
				posessCameraActor(camAct);
			}
			
			void killEvent() {
				std::cout << "Example script ended!\n";
			}
		};

		class InGameCameraPanScript : public wb3d::ActingActor {
		public:
			void beginEvent() {}
			void endEvent() {}

			void tickEvent(float dt) {
				getComponent<Components::TransformComponent>().translation.z -= dt;
				getComponent<Components::TransformComponent>().rotation.x += dt * 0.15;
				getComponent<Components::TransformComponent>().rotation.y += dt * 0.15;
				getComponent<Components::CameraComponent>().camera.setViewYXZ(getComponent<Components::TransformComponent>().translation, getComponent<Components::TransformComponent>().rotation);
			}

			void spawnEvent() {
				getComponent<Components::TransformComponent>().translation = {1.f, -5.f, 6.f};
			}

			void killEvent() {
				std::cout << "Example script ended!\n";
			}
		};
	}
}