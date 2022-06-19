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
#include "input/editor/editor_keyboard_movement_controller.hpp"
#include "input/editor/editor_mouse_movement_controller.hpp"
#include "camera.hpp"
#include "utils/definitions.hpp"
#include "wb3d/master_manager.hpp"

#include "simpleini/simple_ini.h"
#include "buffer.hpp"

//systems
#include "systems/basic_render_system.hpp"

#include "systems/pointlight_system.hpp"
#include "systems/spotlight_system.hpp"
#include "systems/directional_light_system.hpp"

#include "systems/grid_system.hpp"

#include "systems/compute_system.hpp" //mostly post processing prob

//UI stuff
#include "UI/TestLayer.hpp"
#include "UI/ImGuiLayer.hpp"

//scripts
#include "scripts/script_link.h"

namespace Shard3D {
	wb3d::Actor light2{};
	RunApp::RunApp() {
		globalPool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		std::cout << "attempting to construct Level Pointer\n";
		activeLevel = std::make_shared<Level>("runtime test lvl");
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
		ComputeSystem computeSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		BasicRenderSystem basicRenderSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem pointlightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem spotlightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem directionalLightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		std::cout << "Loading editor camera actor\n";
		wb3d::Actor cameraActor = activeLevel->createActorWithGUID(0, "Camera Actor (SYSTEM RESERVED)");
		cameraActor.addComponent<Components::CameraComponent>();
		cameraActor.getComponent<Components::TransformComponent>().translation = glm::vec3(0.f, -1.f, -1.f);

		activeLevel->setPossessedCameraActor(cameraActor);

		loadGameObjects();

		controller::EditorKeyboardMovementController editorCameraControllerKeyboard{};
		controller::EditorMouseMovementController editorCameraControllerMouse{};

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

			auto possessedCamera = activeLevel->getPossessedCameraActor();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			activeLevel->runGarbageCollector(engineDevice.device());
			wb3d::MasterManager::executeQueue(activeLevel, engineDevice);
			activeLevel->tick(frameTime);

			if (activeLevel->simulationState != PlayState::Simulating) {
				editorCameraControllerKeyboard.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, possessedCamera);
				editorCameraControllerMouse.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, possessedCamera);
			}

			cameraActor.getComponent<Components::CameraComponent>().fov = ini.GetDoubleValue("RENDERING", "FOV");
			cameraActor.getComponent<Components::CameraComponent>().ar = engineRenderer.getAspectRatio();

			activeLevel->getPossessedCamera().setViewYXZ(possessedCamera.getComponent<Components::TransformComponent>().translation, possessedCamera.getComponent<Components::TransformComponent>().rotation);
			
			if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") {
				possessedCamera.getComponent<Components::CameraComponent>().projectionType = cameraActor.getComponent<Components::CameraComponent>().Perspective;
				possessedCamera.getComponent<Components::CameraComponent>().setProjection();
			} else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") {
				possessedCamera.getComponent<Components::CameraComponent>().projectionType = cameraActor.getComponent<Components::CameraComponent>().Orthographic;  //Ortho perspective (not needed 99.99% of the time)
				possessedCamera.getComponent<Components::CameraComponent>().setProjection();
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
				    activeLevel->getPossessedCamera(),
					globalDescriptorSets[frameIndex]
				};

				//	update
				GlobalUbo ubo{};
				ubo.projection = activeLevel->getPossessedCamera().getProjection();
				ubo.view = activeLevel->getPossessedCamera().getView();
				ubo.inverseView = activeLevel->getPossessedCamera().getInverseView();

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

				computeSystem.render(frameInfo);

				// Layer overlays
				for (Layer* layer : layerStack) {
					layer->update(commandBuffer, engineWindow.getGLFWwindow(), frameTime, activeLevel);
				}
				engineRenderer.endSwapChainRenderPass(commandBuffer);
				engineRenderer.endFrame();
			}
		}
		if (activeLevel->simulationState != PlayState::Stopped) activeLevel->end();
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

		std::shared_ptr<EngineModel> model = EngineModel::createModelFromFile(engineDevice, "assets/modeldata/FART.obj", ModelType::MODEL_TYPE_OBJ, false); //dont index because model breaks


		model = EngineModel::createModelFromFile(engineDevice, "assets/modeldata/quad.obj", ModelType::MODEL_TYPE_OBJ);

		wb3d::Actor quad = activeLevel->createActor();
		quad.addComponent<Components::MeshComponent>(model);
		quad.getComponent<Components::TransformComponent>().translation = { 0.0f, 0.9f, 0.0f };
		quad.getComponent<Components::TransformComponent>().scale = { 100.f, 1.f, 100.f };
		quad.getComponent<Components::TransformComponent>().rotation = { 0.f, 0.f, 0.f };


		model = EngineModel::createModelFromFile(engineDevice, "assets/modeldata/axis.obj", ModelType::MODEL_TYPE_OBJ, false);

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
		model = EngineModel::createModelFromFile(engineDevice, "assets/modeldata/cone.obj", ModelType::MODEL_TYPE_OBJ, false);


		activeLevel->killActor(axis);

		light2 = activeLevel->createActor();
		light2.addComponent<Components::SpotlightComponent>();
		light2.addComponent<Components::MeshComponent>(model);
		light2.getComponent<Components::TransformComponent>().scale = { 0.1f, 0.1f, 0.1f };
		light2.getComponent<Components::TransformComponent>().translation = { 5.0f, -1.0f, 0.0f };
		light2.getComponent<Components::TransformComponent>().rotation = glm::vec3(1.f, -0.f, -1.f);
		light2.getComponent<Components::SpotlightComponent>().color = { 1.0f, 1.0f, 1.0f };
		light2.getComponent<Components::SpotlightComponent>().lightIntensity = 1.0f;

		activeLevel->killActor(light2);


		activeLevel->killActor(light0);
		activeLevel->killActor(quad);


		model = EngineModel::createModelFromFile(engineDevice, "assets/modeldata/cone.obj", ModelType::MODEL_TYPE_OBJ);

		wb3d::Actor cool = activeLevel->createActor("parent actor test");
		cool.addComponent<Components::MeshComponent>(model);
		cool.addComponent<Components::CppScriptComponent>().bind<CppScripts::ExampleCppScript>();

		wb3d::Actor epic = activeLevel->createActor("camerabeamer");
		epic.addComponent<Components::CameraComponent>().setProjection();
		epic.getComponent<Components::TransformComponent>().translation = {2.f, -2.f, 0.f};
		
		
		//activeLevel->possessedCam = epic.getComponent<Components::CameraComponent>().camera;
	}
}