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
#include "UI/GUILayer.hpp"
//scripts
#include "scripts/script_link.h"
#include "graphics_settings.hpp"
#include "UI/imgui_initter.hpp"
#include "wb3d/assetmgr.hpp"
//#include "systems/fullscreen_renderer.hpp"
#ifndef NDEBUG
#include <video_decode.h>
#endif
#include "scripts/dynamic_script_engine_linker.hpp"

namespace Shard3D {
	EditorApp::EditorApp() {
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
#ifndef _DEPLOY
		if (ini.GetBoolValue("MISC", "enableIntroVideoNonDeploy") == true) {
#ifndef NDEBUG
			VideoPlaybackEngine::EngineH264Video videoEngine;
			videoEngine.createVideoSession(Singleton::engineWindow.getGLFWwindow(), "assets/mediadata/video.wmw");
			while (videoEngine.isPlaying()) {
				glfwPollEvents();
			}
#elif NDEBUG
			SHARD3D_NOIMPL;
#endif
		}
#elif _DEPLOY
		//VideoPlaybackEngine::EngineH264Video videoEngine;
		//videoEngine.createVideoSession(window, ini.GetValue("MISC", "introVideo"));
		//while (videoEngine.isPlaying()) {
		//	glfwPollEvents();
		//}
#endif
		AssetManager::clearAllAssets();
		_special_assets::_editor_icons_load();
		SharedPools::constructPools(Singleton::engineDevice);
		GraphicsSettings::init(&Singleton::engineWindow);
		SHARD3D_INFO("Constructing Level Pointer");
		Singleton::activeLevel = std::make_shared<Level>("runtime test lvl");
		ini.LoadFile(EDITOR_SETTINGS_PATH);
		ImGuiInitter::init(ini.GetBoolValue("THEME", "useLightMode"));
	}
	EditorApp::~EditorApp() { }

	void buttonClickYay() {
		SHARD3D_NOIMPL;
	}
	void button3ClickYay() {
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



#if ENSET_ENABLE_WORLDBUILDER3D
		ImGuiLayer* imguiLayer = new ImGuiLayer();
		layerStack.pushOverlay(imguiLayer);
#endif
		GUILayer* guiLayer0 = new GUILayer();
		GUILayer* guiLayer1 = new GUILayer();
		GUILayer* guiLayer2 = new GUILayer();
		GUILayer* guiLayer3 = new GUILayer();

		layerStack.pushOverlay(guiLayer3);
		layerStack.pushOverlay(guiLayer2);
		layerStack.pushOverlay(guiLayer1);
		layerStack.pushOverlay(guiLayer0);


		std::shared_ptr<GUI::Element> test = std::make_shared<GUI::Element>();
		test->clickEventCallback = buttonClickYay;

		//guiLayer0->attach(Singleton::mainOffScreen.getRenderPass(), &layerStack);
		// 
		// 
		// TODO: render the GUILayer to a seperate renderpass, then render that over the mainoffscreen in the editor viewport, 
		// but render the GUI seperately from everything in the GUIEditor window.

		GridSystem gridSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#if ENSET_ENABLE_COMPUTE_SHADERS
		ComputeSystem computeSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#endif
		BasicRenderSystem basicRenderSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		BillboardRenderSystem billboardRenderSystem{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		_EditorBillboardRenderer editorBillboardRenderer{ Singleton::engineDevice, Singleton::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem pointlightSystem{};
		SpotlightSystem spotlightSystem{};
		DirectionalLightSystem directionalLightSystem{};

//		FullscreenRenderer renderScreen{ Singleton::engineRenderer.getSwapChainRenderPass() };

#if ENSET_ALLOW_PREVIEW_CAMERA
		GridSystem pgridSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		BasicRenderSystem pbasicRenderSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem ppointlightSystem{ Singleton::engineDevice,Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem pspotlightSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem pdirectionalLightSystem{ Singleton::engineDevice, Singleton::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#endif


		SHARD3D_INFO("Loading editor camera actor");
		Actor editor_cameraActor = Singleton::activeLevel->createActorWithGUID(0, "Editor Camera Actor (SYSTEM RESERVED)");
		editor_cameraActor.addComponent<Components::CameraComponent>();

		Singleton::activeLevel->setPossessedCameraActor(editor_cameraActor);
		editor_cameraActor.getComponent<Components::TransformComponent>().setTranslation(glm::vec3(0.f, -1.f, 1.f));
		SHARD3D_INFO("Loading dummy actor");
		Actor dummy = Singleton::activeLevel->createActorWithGUID(1, "Dummy Actor (SYSTEM RESERVED)");

#if ENSET_ALLOW_PREVIEW_CAMERA
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
		editor_cameraActor.getComponent<Components::CameraComponent>().setFOV(ini.GetDoubleValue("RENDERING", "FOV"));
		
		if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") {
			editor_cameraActor.getComponent<Components::CameraComponent>().setProjectionType(editor_cameraActor.getComponent<Components::CameraComponent>().Perspective);
		} else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") {
			editor_cameraActor.getComponent<Components::CameraComponent>().setProjectionType(editor_cameraActor.getComponent<Components::CameraComponent>().Orthographic);  //Ortho perspective (not needed 99.99% of the time)
		}

		GUILayer* layerList[4]{
			guiLayer0,
			guiLayer1,
			guiLayer2,
			guiLayer3
		};
		imguiLayer->attachGUIEditorInfo(layerList);

		auto currentTime = std::chrono::high_resolution_clock::now();
beginWhileLoop:
		while (!Singleton::engineWindow.shouldClose()) {
			glfwPollEvents();
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			auto possessedCameraActor = Singleton::activeLevel->getPossessedCameraActor();
			auto& possessedCamera = Singleton::activeLevel->getPossessedCamera();
			editor_cameraActor = Singleton::activeLevel->getActorFromGUID(0);
#if ENSET_ALLOW_PREVIEW_CAMERA
			auto possessedPreviewCameraActor = Singleton::activeLevel->getPossessedPreviewCameraActor();
			auto possessedPreviewCamera = Singleton::activeLevel->getPossessedPreviewCamera();
#endif

			Singleton::activeLevel->runGarbageCollector(Singleton::engineDevice);
			wb3d::MasterManager::executeQueue(Singleton::activeLevel, Singleton::engineDevice);
			if (Singleton::activeLevel->simulationState == PlayState::Simulating) Singleton::activeLevel->tick(frameTime);
			EngineAudio::globalUpdate(possessedCameraActor.getTransform().getTranslation(), 
				possessedCameraActor.getTransform().getRotation());
			
			if (Singleton::activeLevel->simulationState != PlayState::Simulating) {
				editorCameraControllerKeyboard.moveInPlaneXY(Singleton::engineWindow.getGLFWwindow(), frameTime, editor_cameraActor);
				editorCameraControllerMouse.moveInPlaneXY(Singleton::engineWindow.getGLFWwindow(), frameTime, editor_cameraActor);
			}
			possessedCameraActor.getComponent<Components::CameraComponent>().ar = tempInfo::aspectRatioWoH;
			possessedCamera.setViewYXZ(possessedCameraActor.getTransform().getTranslation(), possessedCameraActor.getTransform().getRotation());
			possessedCameraActor.getComponent<Components::CameraComponent>().setProjection();
#if ENSET_ALLOW_PREVIEW_CAMERA
			possessedPreviewCameraActor.getComponent<Components::CameraComponent>().ar = Singleton::engineRenderer.getAspectRatio();
			possessedPreviewCamera.setViewYXZ(
				possessedPreviewCameraActor.getComponent<Components::TransformComponent>().translation,
				possessedPreviewCameraActor.getComponent<Components::TransformComponent>().rotation
			);
			possessedPreviewCameraActor.getComponent<Components::CameraComponent>().setProjection();
#endif
			
			if (glfwGetKey(Singleton::engineWindow.getGLFWwindow(), GLFW_KEY_F11) == GLFW_PRESS) {
#ifndef NDEBUG
				MessageDialogs::show("No fullscreen in Debug Mode allowed!", "Debug", MessageDialogs::OPTICONERROR);
#endif
//#ifndef NDEBUG
				GraphicsSettings::toggleFullscreen();
//#endif
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

				ubo.materialSettings = Singleton::testPBR;

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

//#ifndef _DEPLOY
				if (Singleton::editorPreviewSettings.ONLY_GAME == false) {
					if (Singleton::editorPreviewSettings.V_EDITOR_BILLBOARDS == true)
						editorBillboardRenderer.render(frameInfo, Singleton::activeLevel);
					if (Singleton::editorPreviewSettings.V_GRID == true)
						gridSystem.render(frameInfo);
				}
//#endif
		//		guiLayer0->update(frameInfo);
				Singleton::mainOffScreen.end(frameInfo);
#if ENSET_ENABLE_COMPUTE_SHADERS
				computeSystem.render(frameInfo);
#endif

#if ENSET_ALLOW_PREVIEW_CAMERA
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
					layer->update(frameInfo);
				}
		
				Singleton::engineRenderer.endSwapChainRenderPass(commandBuffer);
				Singleton::engineRenderer.endFrame();
			}	
			layerStack.popQueue();
		}

		if (MessageDialogs::show("Any unsaved changes will be lost! Are you sure you want to exit?",
			"Shard3D Torque", MessageDialogs::OPTYESNO | MessageDialogs::OPTDEFBUTTON2 | MessageDialogs::OPTICONEXCLAMATION) == MessageDialogs::RESNO) {
			glfwSetWindowShouldClose(Singleton::engineWindow.getGLFWwindow(), GLFW_FALSE);
			goto beginWhileLoop;
		}

		if (Singleton::activeLevel->simulationState != PlayState::Stopped) Singleton::activeLevel->end();

		vkDeviceWaitIdle(Singleton::engineDevice.device());
		wb3d::AssetManager::clearAllAssetsAndDontAddDefaults();
		_special_assets::_editor_icons_destroy();
		
		for (Layer* layer : layerStack) {
			layer->detach();
		}
		SharedPools::destructPools();
		Singleton::viewportImage = nullptr;

		Singleton::activeLevel = nullptr;
	}

	void EditorApp::loadGameObjects() {
		wb3d::LevelManager levelman(Singleton::activeLevel);
		levelman.load("assets/leveldata/sandboox.wbl", true);
		//wb3d::LevelManager levelman(Singleton::activeLevel);
		//levelman.load("assets/leveldata/drivecartest.wbl", true);
		//
		//wb3d::Actor car = Singleton::activeLevel->createActorWithGUID(43827493259, "Car");
		//wb3d::AssetManager::emplaceMesh("assets/modeldata/FART.obj");
		//car.addComponent<Components::MeshComponent>("assets/modeldata/FART.obj");
		//
		//car.getComponent<Components::TransformComponent>().setRotation({ 0.f, 0.f, glm::radians(90.f) });
		//car.addComponent<Components::CppScriptComponent>().bind<CppScripts::CarController>();
		//car.addComponent<Components::AudioComponent>().file = 
		//	"assets/audiodata/race_engine_nb.wav";
		//
		////car.addComponent<Components::ScriptComponent>();
		//
		//wb3d::Actor light = Singleton::activeLevel->createActor("bling");
		//light.getComponent<Components::TransformComponent>().setTranslation({ 0.f, -5.f, 2.f });
		//light.addComponent<Components::PointlightComponent>().color = { 1.f, 0.f, 1.f };
	}
}