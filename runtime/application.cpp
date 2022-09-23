#include "application.h"

#include <thread>
#include <Shard3D/systems/systems.h>

#include <Shard3D/core/asset/assetmgr.h>
#include <Shard3D/core/ui/hud_layer.h>

#include <Shard3D/core/misc/graphics_settings.h>

#include <Shard3D/scripting/script_engine.h>
#include <Shard3D/scripting/script_handler.h>

#include <Shard3D/workarounds.h>
#include <Shard3D/core/ecs/levelmgr.cpp>

namespace Shard3D {
	Application::Application() {
		createRenderPasses();
		setupEngineFeatures();

		SHARD3D_INFO("Constructing Level Pointer");
	}
	Application::~Application() {
	
	}

	void Application::createRenderPasses() {
		
	}

	void Application::destroyRenderPasses() {

	}

	void Application::setupEngineFeatures() {
		EngineAudio::init();
		GraphicsSettings::init(nullptr);
		ScriptEngine::init();

		Input::setWindow(engineWindow);
		setWindowCallbacks();

		SharedPools::constructPools(engineDevice);

		AssetManager::setDevice(engineDevice);
		ResourceHandler::init(engineDevice);

		_special_assets::_editor_icons_load();

		GraphicsSettings::init(&engineWindow);

		ShaderSystem::init();
	}

	void Application::setWindowCallbacks() {
		SHARD3D_EVENT_BIND_HANDLER_RFC(engineWindow, Application::eventEvent);
	}

	void Application::eventEvent(Events::Event& e) {
		//SHARD3D_LOG("{0}", e.toString());
	}

	void Application::resizeFrameBuffers(uint32_t newWidth, uint32_t newHeight, void* ppoSystem, void* gbuffer) {
		reinterpret_cast<PostProcessingSystem*>(ppoSystem)->updateDescriptors(reinterpret_cast<GBufferInputData*>(gbuffer));
	}


	void Application::run(char* levelpath) {
		std::vector<uPtr<EngineBuffer>> uboBuffers(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = make_uPtr<EngineBuffer>(
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

		DeferredRenderSystem deferredRenderSystem{ engineDevice, globalSetLayout->getDescriptorSetLayout() };

		GBufferInputData gbuffer = deferredRenderSystem.getGBufferAttachments();
		GridSystem gridSystem{ engineDevice, deferredRenderSystem.getRenderPass()->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PostProcessingSystem ppoSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), &gbuffer };
		ShadowMappingSystem shadowSystem{ engineDevice };

		BillboardRenderSystem billboardRenderSystem{ engineDevice, deferredRenderSystem.getRenderPass()->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PhysicsSystem physicsSystem{};
		LightSystem lightSystem{};
		Synchronization syncDebander{ SynchronizationType::ComputeToGraphics, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };
		syncDebander.addImageBarrier(
			SynchronizationAttachment::None,
			gbuffer.baseRenderedScene->getImage(),
			gbuffer.baseRenderedScene->getImageSubresourceRange(),
			VK_IMAGE_LAYOUT_GENERAL
		);

		ResourceHandler::init(engineDevice);
		level = make_sPtr<ECS::Level>();

		loadStaticObjects();

		SHARD3D_ASSERT(IOUtils::doesFileExist(levelpath) && "Level does not exist!");
		SHARD3D_ASSERT(AssetUtils::discoverAssetType(levelpath) == AssetType::Level && "Item provided is not a level!");

		ECS::LevelManager levelMan(level);
		ECS::LevelMgrResults result = levelMan.load(levelpath, true);
		
		level->setPhysicsSystem(&physicsSystem);
		level->begin();
		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!engineWindow.shouldClose()) {
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			glfwPollEvents();
			
			auto possessedCameraActor = level->getPossessedCameraActor();
			auto& possessedCamera = level->getPossessedCamera();
			
			level->tick(frameTime);
			level->runGarbageCollector(engineDevice);
			ResourceHandler::runGarbageCollector();
			EngineAudio::globalUpdate(possessedCameraActor.getTransform().getTranslation(), 
				possessedCameraActor.getTransform().getRotation());
			possessedCameraActor.getComponent<Components::CameraComponent>().ar = GraphicsSettings::getRuntimeInfo().aspectRatio;
			possessedCamera.setViewYXZ(possessedCameraActor.getTransform().transformMatrix);
			possessedCameraActor.getComponent<Components::CameraComponent>().setProjection();

			if (auto commandBuffer = engineRenderer.beginFrame()) {
				int frameIndex = engineRenderer.getFrameIndex();
				SharedPools::drawPools[frameIndex]->resetPool();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					possessedCamera,
					globalDescriptorSets[frameIndex],
					*SharedPools::drawPools[frameIndex],
					level
				};
				//	update
				GlobalUbo ubo{};
				ubo.projection = possessedCamera.getProjection();
				ubo.view = possessedCamera.getView();
				ubo.inverseView = possessedCamera.getInverseView();

				lightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//	render
				deferredRenderSystem.beginRenderPass(frameInfo);
				deferredRenderSystem.renderDeferred(frameInfo);
				billboardRenderSystem.render(frameInfo);
				deferredRenderSystem.renderForward(frameInfo);
				deferredRenderSystem.endRenderPass(frameInfo);

				ppoSystem.render(frameInfo);
				ppoSystem.renderGammaCorrection(frameInfo);
				syncDebander.syncBarrier(frameInfo.commandBuffer); 
				engineRenderer.beginSwapChainRenderPass(commandBuffer);
				ppoSystem.renderImageFlipForPresenting(frameInfo);
				engineRenderer.endSwapChainRenderPass(commandBuffer);

				engineRenderer.endFrame(newTime);
				if (engineWindow.wasWindowResized()) {
					glm::ivec3 newsize = { engineWindow.getExtent().width, engineWindow.getExtent().height , 1 };
					deferredRenderSystem.resize(newsize);
					syncDebander.clearBarriers();
					syncDebander.addImageBarrier(SynchronizationAttachment::None, gbuffer.baseRenderedScene->getImage(), gbuffer.baseRenderedScene->getImageSubresourceRange(), VK_IMAGE_LAYOUT_GENERAL);
					resizeFrameBuffers(newsize.x, newsize.y, &ppoSystem, &gbuffer);
					engineWindow.resetWindowResizedFlag();
				}
			}
		}

		level->end();

		ShaderSystem::destroy();
		ScriptEngine::destroy();
		vkDeviceWaitIdle(engineDevice.device());

		ResourceHandler::destroy();
		_special_assets::_editor_icons_destroy();
		
		destroyRenderPasses();
		SharedPools::destructPools();

		level = nullptr;
	}

	void Application::loadStaticObjects() {
		
	}
}