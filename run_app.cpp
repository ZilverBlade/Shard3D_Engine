#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

//lib stuff
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <array>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//fmod
#include "FMOD_engine/fmod_core_engine.hpp"
#include "FMOD_engine/fmod_studio_engine.hpp"

//engine
#include "ez_render_system.hpp"
#include "input/keyboard_movement_controller.hpp"
#include "camera.hpp"
#include "run_app.hpp"


namespace shard {
	float fov = 70.f;

	RunApp::RunApp() {
		loadGameObjects();
	}
	RunApp::~RunApp() {}
	void RunApp::run() {
		EzRenderSystem ezRenderSystem{ shardDevice, shardRenderer.getSwapChainRenderPass() };
		ShardCamera camera{};

		auto viewerObject = ShardGameObject::createGameObject();
		controller::KeyboardMovementController cameraController{};

		std::cout << "FOV set to " << fov << " degrees" << std::endl;
		/*
			just some fmod stuff bcos why not
		

		fmod_engine::FMOD_Core_Engine fmodcore;
		fmod_engine::FMOD_Studio_Engine fmodstudio;

		fmodcore.PlaySoundFile("sounddata/9kkkkkkkkkk.wav"); //zyn :)
		fmodstudio.PlayBankEvent("sounddata/FMOD/Desktop", "engines.bank", "event:/carsounds/arrive");
		fmodstudio.PlayBankEvent("sounddata/FMOD/Desktop", "engines.bank", "event:/carsounds/idlerace", "car_level", 0);

		fmodcore.UpdateVolume(0.75);
		fmodcore.UpdatePitch(0.9);
		*/

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!shardWindow.shouldClose()) {
			glfwPollEvents();


			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			cameraController.moveInPlaneXZ(shardWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = shardRenderer.getAspectRatio();
			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			
			camera.setPerspectiveProjection(glm::radians(fov), aspect, 0.1f, 15.f);

			if (auto commandBuffer = shardRenderer.beginFrame()) {
				/*
					this section is great for adding multiple render passes such as :
					- Begin offscreen shadow pass
					- render shadow casting objects
					- end offscreen shadow pass

					Also reflections and Postfx
				*/
				shardRenderer.beginSwapChainRenderPass(commandBuffer); 
				ezRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				shardRenderer.endSwapChainRenderPass(commandBuffer);
				shardRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(shardDevice.device());
	}

	// temporary helper function, creates a 1x1x1 cube centered at offset
	std::unique_ptr<ShardModel> createCubeModel(ShardDevice& device, glm::vec3 offset) {
		std::vector<ShardModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<ShardModel>(device, vertices);
	}


	void RunApp::loadGameObjects() {	
		std::shared_ptr<ShardModel> shardModel = createCubeModel(shardDevice, { .0f, .0f, .0f });

		auto cube = ShardGameObject::createGameObject();
		cube.model = shardModel;
		cube.transform.translation = { .0f, .0f, 1.5f };
		cube.transform.scale = { .5f, .5f, .5f };

		gameObjects.push_back(std::move(cube));
	}
}