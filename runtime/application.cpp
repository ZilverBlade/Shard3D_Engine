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
		{ // Main renderer
			mainColorFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);

			mainDepthFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				engineRenderer.getSwapchain()->findDepthFormat(),
				VK_IMAGE_ASPECT_DEPTH_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Depth
			);
			mainPositionFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);

			mainNormalFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);

			mainMaterialDataFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);

			AttachmentInfo colorAttachmentInfo{};
			colorAttachmentInfo.frameBufferAttachment = mainColorFramebufferAttachment;
			colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			AttachmentInfo depthAttachmentInfo{};
			depthAttachmentInfo.frameBufferAttachment = mainDepthFramebufferAttachment;
			depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;


			AttachmentInfo positionAttachmentInfo{};
			positionAttachmentInfo.frameBufferAttachment = mainPositionFramebufferAttachment;
			positionAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			positionAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;


			AttachmentInfo normalAttachmentInfo{};
			normalAttachmentInfo.frameBufferAttachment = mainNormalFramebufferAttachment;
			normalAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			normalAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			AttachmentInfo materialAttachmentInfo{};
			materialAttachmentInfo.frameBufferAttachment = mainMaterialDataFramebufferAttachment;
			materialAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			materialAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;


			mainRenderpass = new RenderPass(
				engineDevice, {
				colorAttachmentInfo,
				depthAttachmentInfo,
				positionAttachmentInfo,
				normalAttachmentInfo,
				materialAttachmentInfo
				});

			mainFrameBuffer = new FrameBuffer(engineDevice, mainRenderpass->getRenderPass(), { mainColorFramebufferAttachment, mainDepthFramebufferAttachment, mainPositionFramebufferAttachment, mainNormalFramebufferAttachment, mainMaterialDataFramebufferAttachment });
		}
	}

	void Application::destroyRenderPasses() {
		delete mainFrameBuffer;
		delete mainRenderpass;
		delete mainColorFramebufferAttachment;
		delete mainDepthFramebufferAttachment;
		delete mainNormalFramebufferAttachment;
		delete mainMaterialDataFramebufferAttachment;

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

		ForwardRenderSystem forwardRenderSystem { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PostProcessingSystem ppoSystem{ engineDevice, engineRenderer.getSwapChainRenderPass(), {mainColorFramebufferAttachment, mainPositionFramebufferAttachment, mainNormalFramebufferAttachment, mainMaterialDataFramebufferAttachment} };
		ShadowMappingSystem shadowSystem{ engineDevice };

		BillboardRenderSystem billboardRenderSystem { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PhysicsSystem physicsSystem{};
		LightSystem lightSystem{};

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
				mainRenderpass->beginRenderPass(frameInfo, mainFrameBuffer);
				forwardRenderSystem.renderForward(frameInfo);
				billboardRenderSystem.render(frameInfo);
				mainRenderpass->endRenderPass(frameInfo);

				ppoSystem.render(frameInfo);
				ppoSystem.renderGammaCorrection(frameInfo);
				engineRenderer.beginSwapChainRenderPass(commandBuffer);
				ppoSystem.renderImageFlipForPresenting(frameInfo);
				engineRenderer.endSwapChainRenderPass(commandBuffer);

				engineRenderer.endFrame(newTime);
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