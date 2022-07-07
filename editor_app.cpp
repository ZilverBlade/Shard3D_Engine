#include "s3dtpch.h" 
#include "editor_app.hpp"
//lib stuff

#include <thread>
//glm
#include <glm/gtc/constants.hpp>

//engine
#include "input/editor/editor_keyboard_movement_controller.hpp"
#include "input/editor/editor_mouse_movement_controller.hpp"
#include "camera.hpp"
#include "utils/definitions.hpp"
#include "wb3d/master_manager.hpp"
#include "offscreen.hpp"

#include "simpleini/simple_ini.h"
#include "buffer.hpp"

//systems
#include "systems/systems.h"

//UI stuff
#include "UI/ImGuiLayer.hpp"

//scripts
#include "scripts/script_link.h"
#include "graphics_settings.hpp"
#include "UI/imgui_initter.hpp"
#include "wb3d/assetmgr.hpp"


namespace Shard3D {
	EditorApp::

	EditorApp::EditorApp() {
		SharedPools::constructPools(Singleton::engineDevice);
		GraphicsSettings::init(&Singleton::engineWindow);
		SHARD3D_INFO("Constructing Level Pointer");
		Singleton::activeLevel = std::make_shared<Level>("runtime test lvl");
	}
	EditorApp::~EditorApp() {

	}
	void EditorApp::setupDescriptors() {
#if ENABLE_COMPUTE_SHADERS

#endif
	}
	void EditorApp::run() {
		std::vector<std::unique_ptr<EngineBuffer>> uboBuffers(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<EngineBuffer>(
				Singleton::engineDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uboBuffers[i]->map();
		}
		auto globalSetLayout = EngineDescriptorSetLayout::Builder(Singleton::engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();	
		std::vector<VkDescriptorSet> globalDescriptorSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			EngineDescriptorWriter(*globalSetLayout, *SharedPools::globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}
		ImGuiInitter::init();
#if ENABLE_WORLDBUILDER3D
		layerStack.pushOverlay(new ImGuiLayer());
#endif
		GridSystem gridSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#if ENABLE_COMPUTE_SHADERS == true
		ComputeSystem computeSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#endif
		BasicRenderSystem basicRenderSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		BillboardRenderSystem billboardRenderSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem pointlightSystem{ Singleton::engineDevice,Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem spotlightSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem directionalLightSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

#if ALLOW_PREVIEW_CAMERA
		GridSystem pgridSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		BasicRenderSystem pbasicRenderSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem ppointlightSystem{ Singleton::engineDevice,Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem pspotlightSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem pdirectionalLightSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#endif


		SHARD3D_INFO("Loading editor camera actor");
		wb3d::Actor cameraActor = Singleton::activeLevel->createActorWithGUID(0, "Camera Actor (SYSTEM RESERVED)");
		cameraActor.addComponent<Components::CameraComponent>();
		cameraActor.getComponent<Components::TransformComponent>().translation = glm::vec3(0.f, 1.f, -1.f);

		Singleton::activeLevel->setPossessedCameraActor(cameraActor);

		SHARD3D_INFO("Loading dummy actor");
		wb3d::Actor dummy = Singleton::activeLevel->createActorWithGUID(1, "Dummy Actor (SYSTEM RESERVED)");
#if ALLOW_PREVIEW_CAMERA
		dummy.addComponent<Components::CameraComponent>();
		Singleton::activeLevel->setPossessedPreviewCameraActor(dummy); //dummy
#endif // !_DEPLOY

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
		while (!Singleton::engineWindow.shouldClose()) {
			glfwPollEvents();

			auto possessedCameraActor = Singleton::activeLevel->getPossessedCameraActor();
			auto possessedCamera = Singleton::activeLevel->getPossessedCamera();
#if ALLOW_PREVIEW_CAMERA
			auto possessedPreviewCameraActor = Singleton::activeLevel->getPossessedPreviewCameraActor();
			auto possessedPreviewCamera = Singleton::activeLevel->getPossessedPreviewCamera();
#endif
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			Singleton::activeLevel->runGarbageCollector(Singleton::engineDevice.device());
			wb3d::MasterManager::executeQueue(Singleton::activeLevel, Singleton::engineDevice);
			Singleton::activeLevel->tick(frameTime);

			if (Singleton::activeLevel->simulationState != PlayState::Simulating) {
				editorCameraControllerKeyboard.moveInPlaneXZ(Singleton::engineWindow.getGLFWwindow(), frameTime, possessedCameraActor);
				editorCameraControllerMouse.moveInPlaneXZ(Singleton::engineWindow.getGLFWwindow(), frameTime, possessedCameraActor);
			}

			possessedCameraActor.getComponent<Components::CameraComponent>().ar = Singleton::engineRenderer.getAspectRatio();
			possessedCamera.setViewYXZ(
				possessedCameraActor.getComponent<Components::TransformComponent>().translation, 
				possessedCameraActor.getComponent<Components::TransformComponent>().rotation
			);
#if ALLOW_PREVIEW_CAMERA
			possessedPreviewCameraActor.getComponent<Components::CameraComponent>().ar = Singleton::engineRenderer.getAspectRatio();
			possessedPreviewCamera.setViewYXZ(
				possessedPreviewCameraActor.getComponent<Components::TransformComponent>().translation,
				possessedPreviewCameraActor.getComponent<Components::TransformComponent>().rotation
			);
			possessedPreviewCameraActor.getComponent<Components::CameraComponent>().setProjection();
#endif

			if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") {
				possessedCameraActor.getComponent<Components::CameraComponent>().projectionType = cameraActor.getComponent<Components::CameraComponent>().Perspective;
				possessedCameraActor.getComponent<Components::CameraComponent>().setProjection();
			} else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") {
				possessedCameraActor.getComponent<Components::CameraComponent>().projectionType = cameraActor.getComponent<Components::CameraComponent>().Orthographic;  //Ortho perspective (not needed 99.99% of the time)
				possessedCameraActor.getComponent<Components::CameraComponent>().setProjection();
			}
			
			if (glfwGetKey(Singleton::engineWindow.getGLFWwindow(), GLFW_KEY_F11) == GLFW_PRESS) {
#ifndef NDEBUG
				MessageDialogs::show("No fullscreen in Debug Mode allowed!", "Debug", MessageDialogs::OPTICONERROR);
#endif
#ifndef NDEBUG
				GraphicsSettings::toggleFullscreen();
#endif
			}

			if (auto commandBuffer = Singleton::engineRenderer.beginFrame()) {
				int frameIndex = Singleton::engineRenderer.getFrameIndex();
				SharedPools::drawPools[frameIndex]->resetPool();
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

				pointlightSystem.update(frameInfo, ubo, Singleton::activeLevel);
				spotlightSystem.update(frameInfo, ubo, Singleton::activeLevel);
				directionalLightSystem.update(frameInfo, ubo, Singleton::activeLevel);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
		
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

				//	render
				Singleton::mainOffScreen.start(frameInfo);
				basicRenderSystem.renderGameObjects(frameInfo, Singleton::activeLevel);
				billboardRenderSystem.render(frameInfo, Singleton::activeLevel);

				pointlightSystem.render(frameInfo, Singleton::activeLevel);
				spotlightSystem.render(frameInfo, Singleton::activeLevel);
				directionalLightSystem.render(frameInfo, Singleton::activeLevel);

#if ENABLE_COMPUTE_SHADERS == true
				computeSystem.render(frameInfo);
#endif
				gridSystem.render(frameInfo);

				Singleton::mainOffScreen.end(frameInfo);

#if ALLOW_PREVIEW_CAMERA
				GlobalUbo pubo{};
				pubo.projection = possessedPreviewCamera.getProjection();
				pubo.view = possessedPreviewCamera.getView();
				pubo.inverseView = possessedPreviewCamera.getInverseView();
				frameInfo.camera = possessedPreviewCamera;

				ppointlightSystem.update(frameInfo, pubo, Singleton::activeLevel);
				pspotlightSystem.update(frameInfo, pubo, Singleton::activeLevel);
				pdirectionalLightSystem.update(frameInfo, pubo, Singleton::activeLevel);

				uboBuffers[frameIndex]->writeToBuffer(&pubo);
				uboBuffers[frameIndex]->flush();

				Singleton::previewCamOffScreen.start(frameInfo);
				pbasicRenderSystem.renderGameObjects(frameInfo, Singleton::activeLevel);

				ppointlightSystem.render(frameInfo, Singleton::activeLevel);
				pspotlightSystem.render(frameInfo, Singleton::activeLevel);
				pdirectionalLightSystem.render(frameInfo, Singleton::activeLevel);

				//pgridSystem.render(frameInfo);
				Singleton::previewCamOffScreen.end(frameInfo);
#endif
				Singleton::engineRenderer.beginSwapChainRenderPass(commandBuffer);
				// Layer overlays (use UI here)
				for (Layer* layer : layerStack) {
					layer->update(commandBuffer, frameTime);
				}

				Singleton::engineRenderer.endSwapChainRenderPass(commandBuffer);
				Singleton::engineRenderer.endFrame();
			}
			
		}
		if (Singleton::activeLevel->simulationState != PlayState::Stopped) Singleton::activeLevel->end();
		for (Layer* layer : layerStack) {
			layer->detach();
		}
		vkDeviceWaitIdle(Singleton::engineDevice.device());
		SharedPools::destructPools();
		Singleton::viewportImage == nullptr;
	}

	void EditorApp::loadGameObjects() {

		/*
			NOTE:
			As of now, the model loads in as X right, Y forward, Z up, however the transform values still are X right, Z forward, Y up.
			That means that in the editor, the level must save object transform values as (X, Z, Y), otherwise it will be incorrect
		*/
		
		
		wb3d::LevelManager levelman(Singleton::activeLevel);
		levelman.load("assets/scenedata/drivecartest.wbl", true);
		
		wb3d::Actor car = Singleton::activeLevel->createActor("Car");
		wb3d::AssetManager::emplaceMesh("assets/modeldata/FART.obj");
		car.addComponent<Components::MeshComponent>("assets/modeldata/FART.obj");

		car.getComponent<Components::TransformComponent>().rotation = { 0.f, glm::radians(90.f), 0.f };
		car.addComponent<Components::CppScriptComponent>().bind<CppScripts::CarController>();
		{
		wb3d::Actor billboard = Singleton::activeLevel->createActor("Billboard");
		wb3d::AssetManager::emplaceTexture("assets/_engine/tex/null_tex.png", VK_FILTER_NEAREST);
		billboard.addComponent<Components::BillboardComponent>("assets/_engine/tex/null_tex.png");
		billboard.getTransform().translation = { 1.f, 2.f, 0.f };
		}
		{
		wb3d::Actor billboard = Singleton::activeLevel->createActor("Billboard");
		wb3d::AssetManager::emplaceTexture("assets/_engine/tex/axis2d.png");
		billboard.addComponent<Components::BillboardComponent>("assets/_engine/tex/axis2d.png");
		billboard.getTransform().translation = { 1.f, 2.f, 6.f };
		}
	}
}