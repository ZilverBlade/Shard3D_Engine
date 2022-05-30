#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "run_app.hpp"
//lib stuff
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <array>
#include <thread>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//engine
#include "input/keyboard_movement_controller.hpp"
#include "input/mouse_movement_controller.hpp"
#include "camera.hpp"
#include "utils/definitions.hpp"


#include "simpleini/simple_ini.h"
#include "buffer.hpp"

//systems
#include "systems/basic_render_system.hpp"

#include "systems/pointlight_system.hpp"
#include "systems/spotlight_system.hpp"
#include "systems/directional_light_system.hpp"
#include "systems/grid_system.hpp"

//UI stuff
#include "UI/TestLayer.hpp"
#include "UI/ImGuiLayer.hpp"

namespace Shard3D {

	RunApp::RunApp() {
		std::ifstream infile(ENGINE_SETTINGS_PATH);
		assert(infile.good() != false && "Critical error! Engine settings config file not found!");
		std::ifstream infile2(GAME_SETTINGS_PATH);
		assert(infile2.good() != false && "Critical error! Game settings config file not found!");
	
		globalPool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)			
			.build();
		loadGameObjects();
	}
	RunApp::~RunApp() {}

	void RunApp::run() {
		std::vector<std::unique_ptr<EngineBuffer>> uboBuffers(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<EngineBuffer>(
				engineDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			EngineDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		layerStack.pushLayer(new TestLayer(), engineRenderer.getSwapChainRenderPass(), &engineDevice, engineWindow.getGLFWwindow());
#if ENABLE_WORLDBUILDER3D
		layerStack.pushOverlay(new ImGuiLayer(), engineRenderer.getSwapChainRenderPass(), &engineDevice, engineWindow.getGLFWwindow());
#endif
		GridSystem gridSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		BasicRenderSystem basicRenderSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		
		PointlightSystem pointlightSystem { engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem spotlightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem directionalLightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		EngineCamera camera{};

		auto viewerObject = EngineGameObject::createGameObject();
		viewerObject.transform.translation = glm::vec3(0.f, -1.f, -1.f);

		controller::KeyboardMovementController cameraControllerKeyBoard{};
		controller::MouseMovementController cameraControllerMouse{};

		double mousePosX = {};
		double mousePosY = {};

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		CSimpleIniA gini;
		gini.SetUnicode();
		gini.LoadFile(GAME_SETTINGS_PATH);

		float fov = ini.GetDoubleValue("RENDERING", "FOV");
		std::cout << "Default FOV set to " << fov << " degrees" << std::endl;
	
		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!engineWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			cameraControllerKeyBoard.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, viewerObject);
			cameraControllerMouse.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, viewerObject);
			
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = engineRenderer.getAspectRatio();
			
			if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") {
				camera.setPerspectiveProjection(glm::radians(fov), aspect, ini.GetDoubleValue("RENDERING", "NearClipDistance"), ini.GetDoubleValue("RENDERING", "FarClipDistance"));
			} else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") {
				camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, ini.GetDoubleValue("RENDERING", "FarClipDistance"));  //Ortho perspective (not needed 99.99% of the time)
			}

			if (glfwGetKey(engineWindow.getGLFWwindow(), GLFW_KEY_F11) == GLFW_PRESS) {
				glfwGetWindowSize(engineWindow.getGLFWwindow(), &engineWindow.windowWidth, &engineWindow.windowHeight);
				engineWindow.toggleFullscreen();
			}	

			if (auto commandBuffer = engineRenderer.beginFrame()) {
				int frameIndex = engineRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				//	update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();

				pointlightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				spotlightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				directionalLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//	render

				/*
					this section is great for adding multiple render passes such as :
					- Begin offscreen shadow pass
					- render shadow casting objects
					- end offscreen shadow pass
					- UI

					Also reflections and Postfx

					LAYERS MUST BE LOADED LAST!

					Also order absolutely matters, post processing for example must go last
				*/

				
				engineRenderer.beginSwapChainRenderPass(commandBuffer); 

				basicRenderSystem.renderGameObjects(frameInfo);

				pointlightSystem.render(frameInfo);
				spotlightSystem.render(frameInfo);
				directionalLightSystem.render(frameInfo);

				gridSystem.render(frameInfo);

				// Layer overlays
				for (Layer* layer : layerStack) {
					layer->update(commandBuffer, engineWindow.getGLFWwindow(), frameTime);
				}
				engineRenderer.endSwapChainRenderPass(commandBuffer);
				engineRenderer.endFrame();
			}
		}
		for (Layer* layer : layerStack) {
			layer->detach();
		}
		vkDeviceWaitIdle(engineDevice.device());
	}

	void RunApp::loadGameObjects() {	

		std::shared_ptr<EngineModel> model = EngineModel::createModelFromFile(engineDevice, "modeldata/FART.obj", ModelType::MODEL_TYPE_OBJ, false); //dont index because model breaks

		auto fart = EngineGameObject::createGameObject();
		fart.model = model;
		fart.transform.translation = { .0f, 0.0f, 1.5f };
		fart.transform.scale = { .5f, .5f, .5f };
		fart.transform.rotation = { glm::radians(90.f), 0.f, 0.f};
		gameObjects.emplace(fart.getId(), std::move(fart));

		auto fart2 = EngineGameObject::createGameObject();
		fart2.model = model;
		fart2.transform.translation = { 0.0f, 0.0f, 5.5f };
		fart2.transform.scale = { .5f, .5f, .5f };
		fart2.transform.rotation = { glm::radians(90.f), glm::radians(90.f), 0.f };
		gameObjects.emplace(fart2.getId(), std::move(fart2));

		model = EngineModel::createModelFromFile(engineDevice, "modeldata/colored_cube.obj", ModelType::MODEL_TYPE_OBJ);

		auto ccube = EngineGameObject::createGameObject();
		ccube.model = model;
		ccube.transform.translation = { 2.0f, .0f, 1.5f };
		ccube.transform.scale = { .5f, .5f, .5f };
		ccube.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(ccube.getId(), std::move(ccube));

		model = EngineModel::createModelFromFile(engineDevice, "modeldata/sphere.obj", ModelType::MODEL_TYPE_OBJ, false);

		auto sphere = EngineGameObject::createGameObject();
		sphere.model = model;
		sphere.transform.translation = { 3.0f, .0f, 5.5f };
		sphere.transform.scale = { 1.f, 1.f, 1.f };
		sphere.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(sphere.getId(), std::move(sphere));

		model = EngineModel::createModelFromFile(engineDevice, "modeldata/cylinder.obj", ModelType::MODEL_TYPE_OBJ);

		auto cylinder = EngineGameObject::createGameObject();
		cylinder.model = model;
		cylinder.transform.translation = { 5.0f, .0f, 5.5f };
		cylinder.transform.scale = { .5f, .5f, .5f };
		cylinder.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(cylinder.getId(), std::move(cylinder));
		
		model = EngineModel::createModelFromFile(engineDevice, "modeldata/quad.obj", ModelType::MODEL_TYPE_OBJ);

		auto quad = EngineGameObject::createGameObject();
		quad.model = model;
		quad.transform.translation = { 0.0f, 0.9f, 0.0f };
		quad.transform.scale = { 100.f, 1.f, 100.f };
		quad.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(quad.getId(), std::move(quad));
		
		model = EngineModel::createModelFromFile(engineDevice, "modeldata/cone.obj", ModelType::MODEL_TYPE_OBJ);

		auto cone = EngineGameObject::createGameObject();
		cone.model = model;
		cone.transform.translation = { 1.0f, -1.0f, 6.0f };
		cone.transform.scale = { 0.5f, 0.5f, 0.5f };
		cone.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(cone.getId(), std::move(cone));

		auto cone2 = EngineGameObject::createGameObject();
		cone2.model = model;
		cone2.transform.translation = { .0f, -1.0f, 6.0f };
		cone2.transform.scale = { 0.5f, 0.5f, 0.5f };
		cone2.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(cone2.getId(), std::move(cone2));
		
		
		//auto activeScene
		//auto fartThing = 
		//wb3d::Entity fartObj = {}
		{
			auto pointlight = EngineGameObject::makePointlight(1.f);
			pointlight.transform.translation = { 2.0f, -1.0f, 2.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}

		{
			auto pointlight = EngineGameObject::makePointlight(1.f);
			pointlight.transform.translation = { -2.0f, -1.0f, 2.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}

		{
			auto pointlight = EngineGameObject::makePointlight(1.f);
			pointlight.transform.translation = { 0.0f, -1.0f, 0.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}


		{
			auto pointlight = EngineGameObject::makePointlight(1.f);
			pointlight.transform.translation = { 0.0f, -1.0f, 3.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}
	
		{
			auto pointlight = EngineGameObject::makePointlight(0.3f, 0.1, {1.f, 0.f, 1.f});
			pointlight.transform.translation = { 0.0f, -0.2f, 0.2f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}
		
		{
			auto pointlight = EngineGameObject::makePointlight(0.5f, 0.1, { 1.f, 1.f, 0.f }, { 0.0f, -0.2f, 1.3f });
			pointlight.transform.translation = { 1.0f, -0.2f, 0.2f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}
		
		{
			auto spotlight = EngineGameObject::makeSpotlight(1.3f, 3.1, { 1.f, 1.f, 1.f }, { 0.f, glm::radians(90.f), 0.f }, glm::radians(30.f), glm::radians(40.f), {0.f, 0.f, 1.f});
			spotlight.transform.translation = { 4.0f, -0.1f, 0.2f };
			gameObjects.emplace(spotlight.getId(), std::move(spotlight));
		}
		{
			auto spotlight = EngineGameObject::makeSpotlight(2.2f, 3.1, { 1.f, 1.f, 1.f }, { 0.f, glm::radians(60.f), 1.f }, glm::radians(20.f), glm::radians(30.f), { 0.f, 0.f, 1.f });
			spotlight.transform.translation = { 6.0f, -0.1f, 0.2f };
			gameObjects.emplace(spotlight.getId(), std::move(spotlight));
		}
		
		{
			auto directionalLight = EngineGameObject::makeDirectionalLight(0.2f, glm::vec3(1.f, 1.0f, 1.f), {1.f, -3.f, -1.f}, 0.5f);
			directionalLight.transform.translation = { 2.0f, -0.5f, 0.2f };
			gameObjects.emplace(directionalLight.getId(), std::move(directionalLight));
		}
		
	}
}