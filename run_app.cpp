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
#include "utils/dialogs.h"

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
#include "graphics_settings.hpp"


namespace Shard3D {
	RunApp::RunApp() {
		SharedPools::constructPools(engineDevice);
		GraphicsSettings::init(&engineWindow);
		SHARD3D_INFO("Constructing Level Pointer");
		activeLevel = std::make_shared<Level>("runtime test lvl");
	}
	RunApp::~RunApp() {
		SharedPools::destructPools();
	}
	void RunApp::setupDescriptors() {
#if ENABLE_COMPUTE_SHADERS == true
		SharedPools::computePool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
#endif
	}
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
			EngineDescriptorWriter(*globalSetLayout, *SharedPools::globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

#if ENABLE_COMPUTE_SHADERS == true
		VkDescriptorImageInfo computeImageInfo;
		ComputeUbo cUbo{};
	
		computeImageInfo.sampler = cUbo.inputImage;
		computeImageInfo.imageView = engineRenderer.getSwapchainImageView(0);
		computeImageInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto computeSetLayout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT)
			.build();
		std::vector<VkDescriptorSet> computeDescriptorSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < computeDescriptorSets.size(); i++) {
			EngineDescriptorWriter(*computeSetLayout, *computePool)
				.writeImage(0, &computeImageInfo)
				.build(computeDescriptorSets[i]);
		}
#endif

		layerStack.pushLayer(new TestLayer(), engineRenderer.getSwapChainRenderPass(), &engineDevice, engineWindow.getGLFWwindow(), activeLevel);
#if ENABLE_WORLDBUILDER3D
		layerStack.pushOverlay(new ImGuiLayer(), engineRenderer.getSwapChainRenderPass(), &engineDevice, engineWindow.getGLFWwindow(), activeLevel);
#endif
		GridSystem gridSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#if ENABLE_COMPUTE_SHADERS == true
		ComputeSystem computeSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), computeSetLayout->getDescriptorSetLayout() };
#endif
		BasicRenderSystem basicRenderSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem pointlightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem spotlightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem directionalLightSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		SHARD3D_INFO("Loading editor camera actor");
		wb3d::Actor cameraActor = activeLevel->createActorWithGUID(0, "Camera Actor (SYSTEM RESERVED)");
		cameraActor.addComponent<Components::CameraComponent>();
		cameraActor.getComponent<Components::TransformComponent>().translation = glm::vec3(0.f, 1.f, -1.f);

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
		SHARD3D_INFO("Default FOV set to {0} degrees", fov);
		cameraActor.getComponent<Components::CameraComponent>().fov = ini.GetDoubleValue("RENDERING", "FOV");

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!engineWindow.shouldClose()) {
			glfwPollEvents();

			auto possessedCameraActor = activeLevel->getPossessedCameraActor();
			auto possessedCamera = activeLevel->getPossessedCamera();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			activeLevel->runGarbageCollector(engineDevice.device());
			wb3d::MasterManager::executeQueue(activeLevel, engineDevice);
			activeLevel->tick(frameTime);

			if (activeLevel->simulationState != PlayState::Simulating) {
				editorCameraControllerKeyboard.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, possessedCameraActor);
				editorCameraControllerMouse.moveInPlaneXZ(engineWindow.getGLFWwindow(), frameTime, possessedCameraActor);
			}

			cameraActor.getComponent<Components::CameraComponent>().ar = engineRenderer.getAspectRatio();

			possessedCamera.setViewYXZ(
				possessedCameraActor.getComponent<Components::TransformComponent>().translation, 
				possessedCameraActor.getComponent<Components::TransformComponent>().rotation
			);
			
			if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") {
				possessedCameraActor.getComponent<Components::CameraComponent>().projectionType = cameraActor.getComponent<Components::CameraComponent>().Perspective;
				possessedCameraActor.getComponent<Components::CameraComponent>().setProjection();
			} else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") {
				possessedCameraActor.getComponent<Components::CameraComponent>().projectionType = cameraActor.getComponent<Components::CameraComponent>().Orthographic;  //Ortho perspective (not needed 99.99% of the time)
				possessedCameraActor.getComponent<Components::CameraComponent>().setProjection();
			}
			
			if (glfwGetKey(engineWindow.getGLFWwindow(), GLFW_KEY_F11) == GLFW_PRESS) {
#ifndef NDEBUG
				MessageDialogs::show("No fullscreen in Debug Mode allowed!", "Debug", MessageDialogs::OPTICONERROR);
#endif
#ifndef NDEBUG
				GraphicsSettings::toggleFullscreen();
#endif
			}

			if (auto commandBuffer = engineRenderer.beginFrame()) {
				int frameIndex = engineRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					possessedCamera,
					globalDescriptorSets[frameIndex],
					*SharedPools::drawPools[frameIndex],
				};

				//	update
				GlobalUbo ubo{};
				ubo.projection = possessedCamera.getProjection();
				ubo.view = possessedCamera.getView();
				ubo.inverseView = possessedCamera.getInverseView();

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
#if ENABLE_COMPUTE_SHADERS == true
				computeSystem.render(frameInfo);
#endif
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