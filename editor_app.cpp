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
		SharedPools::constructPools(Globals::engineDevice);
		GraphicsSettings::init(&Globals::engineWindow);
		SHARD3D_INFO("Constructing Level Pointer");
		Globals::activeLevel = std::make_shared<Level>("runtime test lvl");
	}
	EditorApp::~EditorApp() {
		//SharedPools::destructPools();
		//Globals::viewportImage == nullptr;
	}
	void EditorApp::setupDescriptors() {
#if ENABLE_COMPUTE_SHADERS == true
		SharedPools::computePool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
#endif
	}
	void EditorApp::run() {
		std::vector<std::unique_ptr<EngineBuffer>> uboBuffers(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<EngineBuffer>(
				Globals::engineDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uboBuffers[i]->map();
		}
		auto globalSetLayout = EngineDescriptorSetLayout::Builder(Globals::engineDevice)
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
		ImGuiInitter::init();
#if ENABLE_WORLDBUILDER3D
		layerStack.pushOverlay(new ImGuiLayer());
#endif
		GridSystem gridSystem{ Globals::engineDevice, Globals::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#if ENABLE_COMPUTE_SHADERS == true
		ComputeSystem computeSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), computeSetLayout->getDescriptorSetLayout() };
#endif
		BasicRenderSystem basicRenderSystem{ Globals::engineDevice, Globals::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		BillboardRenderSystem billboardRenderSystem{ Globals::engineDevice, Globals::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem pointlightSystem{ Globals::engineDevice,Globals::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem spotlightSystem{ Globals::engineDevice, Globals::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem directionalLightSystem{ Globals::engineDevice, Globals::mainOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

#if ALLOW_PREVIEW_CAMERA
		GridSystem pgridSystem{ Globals::engineDevice, Globals::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		BasicRenderSystem pbasicRenderSystem{ Globals::engineDevice, Globals::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointlightSystem ppointlightSystem{ Globals::engineDevice,Globals::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem pspotlightSystem{ Globals::engineDevice, Globals::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem pdirectionalLightSystem{ Globals::engineDevice, Globals::previewCamOffScreen.getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
#endif


		SHARD3D_INFO("Loading editor camera actor");
		wb3d::Actor cameraActor = Globals::activeLevel->createActorWithGUID(0, "Camera Actor (SYSTEM RESERVED)");
		cameraActor.addComponent<Components::CameraComponent>();
		cameraActor.getComponent<Components::TransformComponent>().translation = glm::vec3(0.f, 1.f, -1.f);

		Globals::activeLevel->setPossessedCameraActor(cameraActor);

		SHARD3D_INFO("Loading dummy actor");
		wb3d::Actor dummy = Globals::activeLevel->createActorWithGUID(1, "Dummy Actor (SYSTEM RESERVED)");
#if ALLOW_PREVIEW_CAMERA
		dummy.addComponent<Components::CameraComponent>();
		Globals::activeLevel->setPossessedPreviewCameraActor(dummy); //dummy
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
		while (!Globals::engineWindow.shouldClose()) {
			glfwPollEvents();

			auto possessedCameraActor = Globals::activeLevel->getPossessedCameraActor();
			auto possessedCamera = Globals::activeLevel->getPossessedCamera();
#if ALLOW_PREVIEW_CAMERA
			auto possessedPreviewCameraActor = Globals::activeLevel->getPossessedPreviewCameraActor();
			auto possessedPreviewCamera = Globals::activeLevel->getPossessedPreviewCamera();
#endif
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			Globals::activeLevel->runGarbageCollector(Globals::engineDevice.device());
			wb3d::MasterManager::executeQueue(Globals::activeLevel, Globals::engineDevice);
			Globals::activeLevel->tick(frameTime);

			if (Globals::activeLevel->simulationState != PlayState::Simulating) {
				editorCameraControllerKeyboard.moveInPlaneXZ(Globals::engineWindow.getGLFWwindow(), frameTime, possessedCameraActor);
				editorCameraControllerMouse.moveInPlaneXZ(Globals::engineWindow.getGLFWwindow(), frameTime, possessedCameraActor);
			}

			possessedCameraActor.getComponent<Components::CameraComponent>().ar = Globals::engineRenderer.getAspectRatio();
			possessedCamera.setViewYXZ(
				possessedCameraActor.getComponent<Components::TransformComponent>().translation, 
				possessedCameraActor.getComponent<Components::TransformComponent>().rotation
			);
#if ALLOW_PREVIEW_CAMERA
			possessedPreviewCameraActor.getComponent<Components::CameraComponent>().ar = Globals::engineRenderer.getAspectRatio();
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
			
			if (glfwGetKey(Globals::engineWindow.getGLFWwindow(), GLFW_KEY_F11) == GLFW_PRESS) {
#ifndef NDEBUG
				MessageDialogs::show("No fullscreen in Debug Mode allowed!", "Debug", MessageDialogs::OPTICONERROR);
#endif
#ifndef NDEBUG
				GraphicsSettings::toggleFullscreen();
#endif
			}

			if (auto commandBuffer = Globals::engineRenderer.beginFrame()) {
				int frameIndex = Globals::engineRenderer.getFrameIndex();
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

				pointlightSystem.update(frameInfo, ubo, Globals::activeLevel);
				spotlightSystem.update(frameInfo, ubo, Globals::activeLevel);
				directionalLightSystem.update(frameInfo, ubo, Globals::activeLevel);
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
				Globals::mainOffScreen.start(frameInfo);
				basicRenderSystem.renderGameObjects(frameInfo, Globals::activeLevel);
				billboardRenderSystem.render(frameInfo, Globals::activeLevel);

				pointlightSystem.render(frameInfo, Globals::activeLevel);
				spotlightSystem.render(frameInfo, Globals::activeLevel);
				directionalLightSystem.render(frameInfo, Globals::activeLevel);

				gridSystem.render(frameInfo);
#if ENABLE_COMPUTE_SHADERS == true
				computeSystem.render(frameInfo);
#endif
				Globals::mainOffScreen.end(frameInfo);

#if ALLOW_PREVIEW_CAMERA
				GlobalUbo pubo{};
				pubo.projection = possessedPreviewCamera.getProjection();
				pubo.view = possessedPreviewCamera.getView();
				pubo.inverseView = possessedPreviewCamera.getInverseView();
				frameInfo.camera = possessedPreviewCamera;

				ppointlightSystem.update(frameInfo, pubo, Globals::activeLevel);
				pspotlightSystem.update(frameInfo, pubo, Globals::activeLevel);
				pdirectionalLightSystem.update(frameInfo, pubo, Globals::activeLevel);

				uboBuffers[frameIndex]->writeToBuffer(&pubo);
				uboBuffers[frameIndex]->flush();

				Globals::previewCamOffScreen.start(frameInfo);
				pbasicRenderSystem.renderGameObjects(frameInfo, Globals::activeLevel);

				ppointlightSystem.render(frameInfo, Globals::activeLevel);
				pspotlightSystem.render(frameInfo, Globals::activeLevel);
				pdirectionalLightSystem.render(frameInfo, Globals::activeLevel);

				//pgridSystem.render(frameInfo);
				Globals::previewCamOffScreen.end(frameInfo);
#endif
				Globals::engineRenderer.beginSwapChainRenderPass(commandBuffer);
				// Layer overlays (use UI here)
				for (Layer* layer : layerStack) {
					layer->update(commandBuffer, frameTime);
				}

				Globals::engineRenderer.endSwapChainRenderPass(commandBuffer);
				Globals::engineRenderer.endFrame();
			}
			
		}
		if (Globals::activeLevel->simulationState != PlayState::Stopped) Globals::activeLevel->end();
		for (Layer* layer : layerStack) {
			layer->detach();
		}
		vkDeviceWaitIdle(Globals::engineDevice.device());
		SharedPools::destructPools();
		Globals::viewportImage == nullptr;
	}

	void EditorApp::loadGameObjects() {

		/*
			NOTE:
			As of now, the model loads in as X right, Y forward, Z up, however the transform values still are X right, Z forward, Y up.
			That means that in the editor, the level must save object transform values as (X, Z, Y), otherwise it will be incorrect
		*/
		
		
		wb3d::LevelManager levelman(Globals::activeLevel);
		levelman.load("assets/scenedata/drivecartest.wbl", true);
		
		wb3d::Actor car = Globals::activeLevel->createActor("Car");
		wb3d::AssetManager::emplaceModel("assets/modeldata/FART.obj");
		car.addComponent<Components::MeshComponent>("assets/modeldata/FART.obj");

		car.getComponent<Components::TransformComponent>().rotation = { 0.f, glm::radians(90.f), 0.f };
		car.addComponent<Components::CppScriptComponent>().bind<CppScripts::CarController>();

		wb3d::Actor billboard = Globals::activeLevel->createActor("Billboard");
		wb3d::AssetManager::emplaceTexture("assets/texturedata/null_tex.png");
		billboard.addComponent<Components::BillboardComponent>("assets/texturedata/null_tex.png");
		billboard.getTransform().translation = { 1.f, 0.f, 2.f };
	}
}