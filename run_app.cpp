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
	wb3d::Actor light2{};
	RunApp::RunApp() {
		std::ifstream infile(ENGINE_SETTINGS_PATH);
		assert(infile.good() != false && "Critical error! Engine settings config file not found!");
		std::ifstream infile2(GAME_SETTINGS_PATH);
		assert(infile2.good() != false && "Critical error! Game settings config file not found!");
	
		globalPool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)			
			.build();
		std::cout << "attempting to construct Level Pointer\n";
		activeLevel = std::make_shared<Level>();

		//loadGameObjects();
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

		layerStack.pushLayer(new TestLayer(), engineRenderer.getSwapChainRenderPass(), &engineDevice, engineWindow.getGLFWwindow(), activeLevel);
#if ENABLE_WORLDBUILDER3D
		layerStack.pushOverlay(new ImGuiLayer(), engineRenderer.getSwapChainRenderPass(), &engineDevice, engineWindow.getGLFWwindow(), activeLevel);
#endif
		GridSystem gridSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		BasicRenderSystem basicRenderSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		
		PointlightSystem pointlightSystem { engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem spotlightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem directionalLightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		
		EngineCamera camera{};
		
		std::cout << "Loading editor camera actor\n";
		wb3d::Actor cameraActor = activeLevel->createActorWithGUID(0, "Camera Actor (SYSTEM RESERVED)");

		//cameraActor.addComponent<Components::TransformComponent>();
		cameraActor.getComponent<Components::TransformComponent>().translation = glm::vec3(0.f, -1.f, -1.f);

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

			cameraControllerKeyBoard.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, cameraActor);
			cameraControllerMouse.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, cameraActor);
			
			camera.setViewYXZ(cameraActor.getComponent<Components::TransformComponent>().translation, cameraActor.getComponent<Components::TransformComponent>().rotation);

			//light2.getComponent<Components::TransformComponent>().translation.x += frameTime;

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
					globalDescriptorSets[frameIndex]
				};

				//	update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				
				pointlightSystem.update(frameInfo, ubo, activeLevel);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				spotlightSystem.update(frameInfo, ubo, activeLevel);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				directionalLightSystem.update(frameInfo, ubo, activeLevel);
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

				basicRenderSystem.renderGameObjects(frameInfo, activeLevel);
				
				pointlightSystem.render(frameInfo, activeLevel);
				spotlightSystem.render(frameInfo, activeLevel);
				directionalLightSystem.render(frameInfo, activeLevel);
				
				gridSystem.render(frameInfo);

				// Layer overlays
				for (Layer* layer : layerStack) {
					layer->update(commandBuffer, engineWindow.getGLFWwindow(), frameTime, activeLevel);
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

		/*
			NOTE:
			As of now, the model loads in as X right, Y forward, Z up, however the transform values still are X right, Z forward, -Y up.
			That means that in the editor, the level must save object transform values as (X, -Z, Y), otherwise it will be incorrect
		*/
		
		std::shared_ptr<EngineModel> model = EngineModel::createModelFromFile(engineDevice, "modeldata/FART.obj", ModelType::MODEL_TYPE_OBJ, false); //dont index because model breaks

		wb3d::Actor fartObj = activeLevel->createActor();
		fartObj.addComponent<Components::MeshComponent>(model);

		fartObj.getComponent<Components::TransformComponent>().translation = {0.f, 0.f, 0.f};
		fartObj.getComponent<Components::TransformComponent>().scale = { .5f, .5f, .5f };
		fartObj.getComponent<Components::TransformComponent>().rotation = { 0.f, 0.f, 0.f };
		std::shared_ptr<EngineModel> mode2l = EngineModel::createModelFromFile(engineDevice, "modeldata/cone.obj", ModelType::MODEL_TYPE_OBJ, false); //dont index because model breaks

		fartObj.getComponent<Components::MeshComponent>().reapplyModel(mode2l);

		model = EngineModel::createModelFromFile(engineDevice, "modeldata/quad.obj", ModelType::MODEL_TYPE_OBJ);

		wb3d::Actor quad = activeLevel->createActor();
		quad.addComponent<Components::MeshComponent>(model);
		quad.getComponent<Components::TransformComponent>().translation = { 0.0f, 0.9f, 0.0f };
		quad.getComponent<Components::TransformComponent>().scale = { 100.f, 1.f, 100.f };
		quad.getComponent<Components::TransformComponent>().rotation = { 0.f, 0.f, 0.f };


		model = EngineModel::createModelFromFile(engineDevice, "modeldata/axis.obj", ModelType::MODEL_TYPE_OBJ, false);

		wb3d::Actor axis = activeLevel->createActor();
		axis.addComponent<Components::MeshComponent>(model);
		axis.getComponent<Components::TransformComponent>().translation = { 0.0f, 0.0f, 0.0f };
		axis.getComponent<Components::TransformComponent>().scale = { 1.f, 1.f, 1.f };
		axis.getComponent<Components::TransformComponent>().rotation = { 0.f, 0.f, 0.f };



		wb3d::Actor light = activeLevel->createActor();
		light.addComponent<Components::PointlightComponent>();
		light.getComponent<Components::TransformComponent>().translation = { 2.0f, -1.0f, 0.0f };
		light.getComponent<Components::TransformComponent>().rotation = glm::vec3(1.f, -3.f, -1.f);
		light.getComponent<Components::PointlightComponent>().color = { 1.0f, 1.0f, 1.0f };
		light.getComponent<Components::PointlightComponent>().lightIntensity = 1.0f;


		wb3d::Actor light0 = activeLevel->createActor();
		light0.addComponent<Components::DirectionalLightComponent>();
		light0.getComponent<Components::TransformComponent>().translation = { 0.0f, -1.0f, 5.0f };
		light0.getComponent<Components::TransformComponent>().rotation = glm::vec3(1.f, -3.f, -1.f);
		light0.getComponent<Components::DirectionalLightComponent>().color = { 1.0f, 1.0f, 1.0f };
		light0.getComponent<Components::DirectionalLightComponent>().lightIntensity = 0.1f;
		light0.getComponent<Components::DirectionalLightComponent>().specularMod = 0.0f;
		model = EngineModel::createModelFromFile(engineDevice, "modeldata/cone.obj", ModelType::MODEL_TYPE_OBJ, false);

		light2 = activeLevel->createActor();
		light2.addComponent<Components::SpotlightComponent>();
		light2.addComponent<Components::MeshComponent>(model);
		light2.getComponent<Components::TransformComponent>().scale = { 0.1f, 0.1f, 0.1f };
		light2.getComponent<Components::TransformComponent>().translation = { 5.0f, -1.0f, 0.0f };
		light2.getComponent<Components::TransformComponent>().rotation = glm::vec3(1.f, -0.f, -1.f);
		light2.getComponent<Components::SpotlightComponent>().color = { 1.0f, 1.0f, 1.0f };
		light2.getComponent<Components::SpotlightComponent>().lightIntensity = 1.0f;

		
		//light.getComponent<Components::DirectionalLightComponent>().attenuationMod = {1.f, 1.f, 1.f, 1.f};
		
		/*
		std::shared_ptr<EngineModel> model = EngineModel::createModelFromFile(engineDevice, "modeldata/FART.obj", ModelType::MODEL_TYPE_OBJ, false); //dont index because model breaks
	
		auto fart = EngineGameObject::createGameObject();
		fart.model = model;
		fart.transform.translation = { .0f, 0.0f, 1.5f };
		fart.transform.scale = { .5f, .5f, .5f };
		fart.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(fart.getId(), std::move(fart));

		auto fart2 = EngineGameObject::createGameObject();
		fart2.model = model;
		fart2.transform.translation = { 0.0f, 0.0f, 5.5f };
		fart2.transform.scale = { .5f, .5f, .5f };
		fart2.transform.rotation = { 0.f, 0.f, 0.f };
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

		model = EngineModel::createModelFromFile(engineDevice, "modeldata/axis.obj", ModelType::MODEL_TYPE_OBJ, false);

		auto axis = EngineGameObject::createGameObject();
		axis.model = model;
		axis.transform.translation = { 0.f, 0.f, 0.f };
		axis.transform.scale = { .25f, .25f, .25f };
		axis.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(axis.getId(), std::move(axis));

		model = EngineModel::createModelFromFile(engineDevice, "modeldata/blank_fart.obj", ModelType::MODEL_TYPE_OBJ, false);

		auto refcar = EngineGameObject::createGameObject();
		refcar.model = model;
		refcar.transform.translation = { 0.f, 0.f, 0.f };
		refcar.transform.scale = { .25f, .25f, .25f };
		refcar.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(refcar.getId(), std::move(refcar));


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
		*/
	}
}