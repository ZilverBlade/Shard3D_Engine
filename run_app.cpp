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
#include "input/mouse_movement_controller.hpp"
#include "camera.hpp"
#include "run_app.hpp"
#include "simpleini/simple_ini.h"


namespace shard {
	CSimpleIniA ini;

	RunApp::RunApp() {
		loadGameObjects();
	}
	RunApp::~RunApp() {}
	void RunApp::run() {
		EzRenderSystem ezRenderSystem{ shardDevice, shardRenderer.getSwapChainRenderPass() };
		ShardCamera camera{};

		auto viewerObject = ShardGameObject::createGameObject();
		controller::KeyboardMovementController cameraControllerKeyBoard{};
		controller::MouseMovementController cameraControllerMouse{};

		double mousePosX = {};
		double mousePosY = {};


		ini.SetUnicode();
		ini.LoadFile("settings/engine_settings.ini");

		float fov = ini.GetDoubleValue("DISPLAY", "FOV");
		std::cout << "Default FOV set to " << fov << " degrees" << std::endl;

		/*
			just some fmod stuff bcos why not
		*/

		fmod_engine::FMOD_Core_Engine fmodcore;
		fmod_engine::FMOD_Studio_Engine fmodstudio;

		fmodcore.PlaySoundFile("sounddata/9kkkkkkkkkk.mp3"); //zyn :)
		fmodstudio.PlayBankEvent("sounddata/FMOD/Desktop", "engines.bank", "event:/carsounds/arrive");
		//fmodstudio.PlayBankEvent("sounddata/FMOD/Desktop", "engines.bank", "event:/carsounds/idlerace", "car_level", 0);

		fmodcore.UpdateVolume(0.55);
		fmodcore.UpdatePitch(1);
		

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!shardWindow.shouldClose()) {
			glfwPollEvents();


			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			glfwGetCursorPos(shardWindow.getGLFWwindow(), &mousePosX, &mousePosY);

			cameraControllerKeyBoard.moveInPlaneXZ(shardWindow.getGLFWwindow(), frameTime, viewerObject);
			cameraControllerMouse.moveInPlaneXZ(shardWindow.getGLFWwindow(), frameTime, viewerObject, { mousePosX, mousePosY });
			
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = shardRenderer.getAspectRatio();
			
			if ((std::string)ini.GetValue("DISPLAY", "View") == "Perspective") {
				camera.setPerspectiveProjection(glm::radians(fov), aspect, ini.GetDoubleValue("DISPLAY", "NearClipDistance"), ini.GetDoubleValue("DISPLAY", "FarClipDistance"));
			}
			else if ((std::string)ini.GetValue("DISPLAY", "View") == "Orthographic"){
				camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, ini.GetDoubleValue("DISPLAY", "FarClipDistance"));  //Ortho perspective (not needed 99.99% of the time)
			}
			

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

	

	void RunApp::loadGameObjects() {	
		std::shared_ptr<ShardModel> fart = ShardModel ::createModelFromFile(shardDevice, "modeldata/FART.obj", false); //dont index because model breaks

		auto gameObject = ShardGameObject::createGameObject();
		gameObject.model = fart;
		gameObject.transform.translation = { .0f, .0f, 1.5f };
		gameObject.transform.scale = { .5f, .5f, .5f };
		gameObject.transform.rotation = { glm::radians(90.f), 0.f, 0.f};
		gameObjects.push_back(std::move(gameObject));

		std::shared_ptr<ShardModel> cube = ShardModel::createModelFromFile(shardDevice, "modeldata/colored_cube.obj");

		auto gameObject2 = ShardGameObject::createGameObject();
		gameObject2.model = cube;
		gameObject2.transform.translation = { 2.0f, .0f, 1.5f };
		gameObject2.transform.scale = { .5f, .5f, .5f };
		gameObject2.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.push_back(std::move(gameObject2));

		std::shared_ptr<ShardModel> sphere = ShardModel::createModelFromFile(shardDevice, "modeldata/sphere.obj", false);

		auto gameObject3 = ShardGameObject::createGameObject();
		gameObject3.model = sphere;
		gameObject3.transform.translation = { 3.0f, .0f, 5.5f };
		gameObject3.transform.scale = { 1.45f, .5f, .5f };
		gameObject3.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.push_back(std::move(gameObject3));

		std::shared_ptr<ShardModel> cyl = ShardModel::createModelFromFile(shardDevice, "modeldata/cylinder.obj");

		auto gameObject4 = ShardGameObject::createGameObject();
		gameObject4.model = cyl;
		gameObject4.transform.translation = { 5.0f, .0f, 5.5f };
		gameObject4.transform.scale = { .5f, .5f, .5f };
		gameObject4.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.push_back(std::move(gameObject4));

	}
}