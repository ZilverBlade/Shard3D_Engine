#include "s3dtpch.h" 
#include "Project.hpp"

/*
namespace Shard3D {
	ProjectApp::ProjectApp() {
		SharedPools::constructPools(engineDevice);
		GraphicsSettings::init(&engineWindow);
		SHARD3D_INFO("Constructing Level Pointer");
		activeLevel = std::make_shared<Level>("runtime test lvl");
	}
	ProjectApp::~ProjectApp() {
		SharedPools::destructPools();
	}
	void ProjectApp::setupDescriptors() {
#if ENABLE_COMPUTE_SHADERS == true
		SharedPools::computePool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
#endif
	}
	void ProjectApp::run() {
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
		activeLevel->setPossessedCameraActor(cameraActor); // need default camera actor to prevent the engine dying

		loadGameObjects();

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

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

			possessedCameraActor.getComponent<Components::CameraComponent>().ar = engineRenderer.getAspectRatio();

			possessedCamera.setViewYXZ(
				possessedCameraActor.getComponent<Components::TransformComponent>().translation,
				possessedCameraActor.getComponent<Components::TransformComponent>().rotation
			);

			if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") {
				possessedCameraActor.getComponent<Components::CameraComponent>().projectionType = cameraActor.getComponent<Components::CameraComponent>().Perspective;
				possessedCameraActor.getComponent<Components::CameraComponent>().setProjection();
			}
			else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") {
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
				

				engineRenderer.beginSwapChainRenderPass(commandBuffer);

				basicRenderSystem.renderGameObjects(frameInfo, activeLevel);

				pointlightSystem.render(frameInfo, activeLevel);
				spotlightSystem.render(frameInfo, activeLevel);
				directionalLightSystem.render(frameInfo, activeLevel);

				// Layer overlays
				for (Layer* layer : layerStack) {
					layer->update(commandBuffer, engineWindow.getGLFWwindow(), frameTime, activeLevel);
				}
				engineRenderer.endSwapChainRenderPass(commandBuffer);
				engineRenderer.endFrame();
			}
		}
		if (activeLevel->simulationState != PlayState::Stopped) activeLevel->end();
		vkDeviceWaitIdle(engineDevice.device());
	}

	void ProjectApp::loadGameObjects() {
		std::shared_ptr<EngineModel> model = EngineModel::createModelFromFile("assets/modeldata/FART.obj", ModelType::MODEL_TYPE_OBJ); //dont index because model breaks
		wb3d::LevelManager levelman(activeLevel);
		levelman.load("assets/scenedata/drivecartest.encr.wbl", true);

		wb3d::Actor car = activeLevel->createActor("Car");
		car.addComponent<Components::MeshComponent>(model);

		car.getComponent<Components::TransformComponent>().rotation = { 0.f, glm::radians(90.f), 0.f };
		car.addComponent<Components::CppScriptComponent>().bind<CppScripts::CarController>();
		activeLevel->begin();
	}
}
*/