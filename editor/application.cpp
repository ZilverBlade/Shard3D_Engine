#include "application.h"

#include <thread>
#include <fstream>
#include <vector>
#include <Shard3D/systems/systems.h>

#include <Shard3D/core/asset/assetmgr.h>
#include <Shard3D/core/ui/hud_layer.h>

#include "imgui/imgui_layer.h"
#include "imgui/imgui_initializer.h"

#include <Shard3D/core/misc/graphics_settings.h>

#include <Shard3D/scripting/script_engine.h>
#include <Shard3D/scripting/script_handler.h>

#include <Shard3D/workarounds.h>

// c++ scripts
#include "scripting/script_link.h"

namespace Shard3D {
	Application::Application() {
		createRenderPasses();
		setupEngineFeatures();

	}
	Application::~Application() {
	
	}

	void Application::createRenderPasses() {
		{ // Main renderer
			mainColorFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				{ wndWidth, wndHeight, 1 },
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);

			mainDepthFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				engineRenderer.getSwapchain()->findDepthFormat(),
				VK_IMAGE_ASPECT_DEPTH_BIT,
				{ wndWidth, wndHeight, 1 },
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Depth
			);

#ifdef ENEXP_ENABLE_FORWARD_GBUFFER
			mainPositionFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				{ wndWidth, wndHeight, 1 },
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);

			mainNormalFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				{ wndWidth, wndHeight, 1 },
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);

			mainMaterialDataFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				{ wndWidth, wndHeight, 1 },
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color
			);
#endif
			AttachmentInfo colorAttachmentInfo{};
			colorAttachmentInfo.frameBufferAttachment = mainColorFramebufferAttachment;
			colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			AttachmentInfo depthAttachmentInfo{};
			depthAttachmentInfo.frameBufferAttachment = mainDepthFramebufferAttachment;
			depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;


#ifdef ENEXP_ENABLE_FORWARD_GBUFFER
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

#endif
			mainRenderpass = new RenderPass(
				engineDevice, {
				colorAttachmentInfo,
				depthAttachmentInfo
#ifdef ENEXP_ENABLE_FORWARD_GBUFFER
			,
				positionAttachmentInfo,
				normalAttachmentInfo,
				materialAttachmentInfo
#endif
				});

			mainFrameBuffer = new FrameBuffer(engineDevice, mainRenderpass->getRenderPass(), { mainColorFramebufferAttachment, mainDepthFramebufferAttachment
#ifdef ENEXP_ENABLE_FORWARD_GBUFFER
				, mainPositionFramebufferAttachment, mainNormalFramebufferAttachment, mainMaterialDataFramebufferAttachment 
#endif	
				});
		}
		{ // Post processing
			ppoColorFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
			VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				{ wndWidth, wndHeight, 1 },
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Color);

			AttachmentInfo colorAttachmentInfo{};
			colorAttachmentInfo.frameBufferAttachment = ppoColorFramebufferAttachment;
			colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			ppoRenderpass = new RenderPass(
				engineDevice, {
				colorAttachmentInfo
				});

			ppoFrameBuffer = new FrameBuffer(engineDevice, ppoRenderpass->getRenderPass(), { ppoColorFramebufferAttachment });
		}
		
	}

	void Application::destroyRenderPasses() {
		delete mainFrameBuffer;
		delete mainRenderpass;
		delete mainColorFramebufferAttachment;
		delete mainDepthFramebufferAttachment;

#ifdef ENEXP_ENABLE_FORWARD_GBUFFER
		delete mainPositionFramebufferAttachment;
		delete mainNormalFramebufferAttachment;
		delete mainMaterialDataFramebufferAttachment;
#endif

		delete ppoFrameBuffer;
		delete ppoColorFramebufferAttachment;
		delete ppoRenderpass;
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

	void Application::windowResizeEvent(Events::WindowResizeEvent& e) {
		createRenderPasses();
	}

	void Application::eventEvent(Events::Event& e) {
	}


	void Application::resizeFrameBuffers(uint32_t newWidth, uint32_t newHeight, void* editor, void* ppoSystem) {
		mainFrameBuffer->resize(glm::ivec3(newWidth, newHeight, 1), mainRenderpass->getRenderPass());
		ppoFrameBuffer->resize(glm::ivec3(newWidth, newHeight, 1), ppoRenderpass->getRenderPass());

		ImGuiInitializer::setViewportImage(&reinterpret_cast<ImGuiLayer*>(editor)->viewportImage, ppoColorFramebufferAttachment);
		reinterpret_cast<PostProcessingSystem*>(ppoSystem)->updateDescriptors({mainColorFramebufferAttachment, mainPositionFramebufferAttachment, mainNormalFramebufferAttachment, mainMaterialDataFramebufferAttachment});
	}

	void Application::run() {
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

		
		GridSystem gridSystem { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		ForwardRenderSystem forwardRenderSystem { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PostProcessingSystem ppoSystem { engineDevice, ppoRenderpass->getRenderPass(), {mainColorFramebufferAttachment, mainPositionFramebufferAttachment, mainNormalFramebufferAttachment, mainMaterialDataFramebufferAttachment}};
		ShadowMappingSystem shadowSystem { engineDevice };

		BillboardRenderSystem billboardRenderSystem { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		_EditorBillboardRenderer editorBillboardRenderer { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PhysicsSystem physicsSystem{};
		LightSystem lightSystem{};

		ResourceHandler::init(engineDevice);
		// level must be created after resource handler has been initialised due to the fact that the editor camera is created with post processing materials
		SHARD3D_INFO("Constructing Level Pointer");
		level = make_sPtr<ECS::Level>();
		level->setPhysicsSystem(&physicsSystem);

		{
			CSimpleIniA ini;
			ini.SetUnicode();
			ini.LoadFile(EDITOR_SETTINGS_PATH);

			ImGuiInitializer::init(engineDevice, engineWindow, engineRenderer.getSwapChainRenderPass(), ini.GetBoolValue("THEME", "useLightMode"));
		}
		ImGuiLayer imguiLayer = { engineDevice, engineWindow, engineRenderer.getSwapChainRenderPass() };

		ImGuiInitializer::setViewportImage(&imguiLayer.viewportImage, ppoColorFramebufferAttachment);

		// TODO: render the HUDLayer to a seperate renderpass, then render that over the mainoffscreen in the editor viewport, 
		// but render the HUD seperately from everything in the GUIEditor window.
		HUDLayer hudLayer0{engineDevice, engineWindow, ppoRenderpass->getRenderPass()};
		HUDLayer hudLayer1{engineDevice, engineWindow, ppoRenderpass->getRenderPass()};
		HUDLayer hudLayer2{engineDevice, engineWindow, ppoRenderpass->getRenderPass()};
		HUDLayer hudLayer3{engineDevice, engineWindow, ppoRenderpass->getRenderPass()};
		hudLayer0.layer = 0;
		hudLayer1.layer = 1;
		hudLayer2.layer = 2;
		hudLayer3.layer = 3;

		auto editor_cameraActor = level->getActorFromUUID(0);

		{
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
			}
			else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") {
				editor_cameraActor.getComponent<Components::CameraComponent>().setProjectionType(editor_cameraActor.getComponent<Components::CameraComponent>().Orthographic);  //Ortho perspective (not needed 99.99% of the time)
			}
		}
		//loadStaticObjects();

		HUDLayer* layerList[4]{
			&hudLayer0,
			&hudLayer1,
			&hudLayer2,
			&hudLayer3
		};

		sPtr<HUDContainer> h_l_layer = make_sPtr<HUDContainer>();
		for (int i = 0; i < 4; i++) {
			if (layerList[i])
				h_l_layer->hudLayerList
				.push_back(&layerList[i]->hud);
		}

		
		for (int i = 0; i < h_l_layer->getList().size(); i++) {
			SHARD3D_LOG("HUD Layer {0} has {1} elements", i, h_l_layer->getList().at(i)->elements.size());
		}
		ScriptEngine::setHUDContext(h_l_layer.get());
		imguiLayer.attachGUIEditorInfo(h_l_layer);

		auto currentTime = std::chrono::high_resolution_clock::now();
beginWhileLoop:
		while (!engineWindow.shouldClose()) {
			StatsTimer::dumpIf();
			StatsTimer::clear();
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			SHARD3D_STAT_RECORD();
			glfwPollEvents();
			SHARD3D_STAT_RECORD_END({"Window", "Polling"});
		
			auto possessedCameraActor = level->getPossessedCameraActor();
			auto& possessedCamera = level->getPossessedCamera();
			editor_cameraActor = level->getActorFromUUID(0);

			if (level->simulationState == PlayState::Playing) {
			//	SHARD3D_STAT_RECORD(); 
			//	physicsSystem.simulate(level.get(), frameTime);
			//	SHARD3D_STAT_RECORD_END({ "Level", "Physics" });
				SHARD3D_STAT_RECORD();
				level->tick(frameTime);
				SHARD3D_STAT_RECORD_END({ "Level", "Tick" });
			}
			SHARD3D_STAT_RECORD();
			level->runGarbageCollector(engineDevice);
			ResourceHandler::runGarbageCollector();
			EngineAudio::globalUpdate(possessedCameraActor.getTransform().getTranslation(), 
				possessedCameraActor.getTransform().getRotation());
			SHARD3D_STAT_RECORD_END({ "Engine", "Garbage Collection" });
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
				SHARD3D_STAT_RECORD();
				//	update
				GlobalUbo ubo{};
				ubo.projection = possessedCamera.getProjection();
				ubo.view = possessedCamera.getView();
				ubo.inverseView = possessedCamera.getInverseView();

				
				lightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				SHARD3D_STAT_RECORD_END({"UBO", "Update"});

				//	render
			//	SHARD3D_STAT_RECORD();
			//	shadowSystem.render(frameInfo);
			//	SHARD3D_STAT_RECORD_END({ "Rendering", "Shadow Mapping" });

				mainRenderpass->beginRenderPass(frameInfo, mainFrameBuffer);

				SHARD3D_STAT_RECORD();
				forwardRenderSystem.renderForwardOld(frameInfo);
				SHARD3D_STAT_RECORD_END({ "Forward Pass", "Lighting" });

				SHARD3D_STAT_RECORD();
				billboardRenderSystem.render(frameInfo);

				if (GraphicsSettings::editorPreview.ONLY_GAME == false) {
					if (GraphicsSettings::editorPreview.V_EDITOR_BILLBOARDS == true)
						editorBillboardRenderer.render(frameInfo);
					if (GraphicsSettings::editorPreview.V_GRID == true)
						gridSystem.render(frameInfo);					
				}
				SHARD3D_STAT_RECORD_END({ "Forward Pass", "Billboards" });

				mainRenderpass->endRenderPass(frameInfo);

				//SHARD3D_STAT_RECORD();
				ppoSystem.render(frameInfo);
				//SHARD3D_STAT_RECORD_END({ "Post Processing", "Main" });

				SHARD3D_STAT_RECORD();
				ppoRenderpass->beginRenderPass(frameInfo, ppoFrameBuffer);

				ppoSystem.renderImageFlipForPresenting(frameInfo);

				hudLayer0.render(frameInfo);
				hudLayer1.render(frameInfo);
				hudLayer2.render(frameInfo);
				hudLayer3.render(frameInfo);

				ppoRenderpass->endRenderPass(frameInfo);
				SHARD3D_STAT_RECORD_END({ "Post Processing", "Debanding" });

				imguiLayer.render(frameInfo);
				engineRenderer.beginSwapChainRenderPass(commandBuffer);
				imguiLayer.draw(frameInfo);
				SHARD3D_STAT_RECORD();
				engineRenderer.endSwapChainRenderPass(commandBuffer);
				SHARD3D_STAT_RECORD_END({ "Swapchain", "End" });

				// Command buffer ends
				SHARD3D_STAT_RECORD();
				engineRenderer.endFrame(newTime);
				SHARD3D_STAT_RECORD_END({ "Command Buffer", "Submit" });

				SHARD3D_STAT_RECORD();
				if (engineWindow.wasWindowResized()) {
					resizeFrameBuffers(engineWindow.getExtent().width, engineWindow.getExtent().height, &imguiLayer, &ppoSystem);
					engineWindow.resetWindowResizedFlag();
				}
				SHARD3D_STAT_RECORD_END({ "Window", "Resize check & FBuffer" });

			}
		}

		if (MessageDialogs::show("Any unsaved changes will be lost! Are you sure you want to exit?",
			"Shard3D Torque", MessageDialogs::OPTYESNO | MessageDialogs::OPTDEFBUTTON2 | MessageDialogs::OPTICONEXCLAMATION) == MessageDialogs::RESNO) {
			glfwSetWindowShouldClose(engineWindow.getGLFWwindow(), GLFW_FALSE);
			goto beginWhileLoop;
		}

		if (level->simulationState != PlayState::Stopped) level->end();
		vkDeviceWaitIdle(engineDevice.device());

		imguiLayer.detach();

		for (HUD* my : h_l_layer->getList()) {
			my->elements.clear();
		}

		ShaderSystem::destroy();
		ScriptEngine::destroy();
		
		ResourceHandler::destroy();
		_special_assets::_editor_icons_destroy();
		
		destroyRenderPasses();
		SharedPools::destructPools();

		level = nullptr;
	}

	void Application::loadStaticObjects() {
		
		Actor phys = level->createActor();
		phys.addComponent<Components::Rigidbody3DComponent>();
		auto boxShape = level->physicsSystemPtr->createBoxShape(JPH::BoxShapeSettings(JPH::Vec3(100.0f, 1.0f, 100.0f)));
		phys.getComponent<Components::TransformComponent>().setScale({ 100.f, 100.f, 1.f });
		phys.getComponent<Components::TransformComponent>().setTranslation({ 0.0f, 100.0f, -2.0f });
		phys.getComponent<Components::Rigidbody3DComponent>().physicsBody = 
			level->physicsSystemPtr->createBody(JPH::BodyCreationSettings(boxShape, JPH::Vec3(0.0f, -2.0f, 100.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING));
		
		
		Actor sphere = level->createActor();
		sphere.addComponent<Components::Rigidbody3DComponent>();
		auto sphereShape = level->physicsSystemPtr->createSphereShape(JPH::SphereShapeSettings(0.5f));
		sphere.getComponent<Components::TransformComponent>().setScale({ 0.5f, 0.5f, 0.5f });
		sphere.getComponent<Components::Rigidbody3DComponent>().physicsBody =
			level->physicsSystemPtr->createBody(JPH::BodyCreationSettings(sphereShape, JPH::Vec3(0.0f, 2.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING));
		sphere.getComponent<Components::TransformComponent>().setTranslation({ 0.f, 0.f, 2.f });
		level->physicsSystemPtr->getInterface().AddLinearVelocity(sphere.getComponent<Components::Rigidbody3DComponent>().physicsBody, { 1.f, 0.f, 1.f });
		level->physicsSystemPtr->getInterface().SetGravityFactor(sphere.getComponent<Components::Rigidbody3DComponent>().physicsBody, 1.f);
		level->physicsSystemPtr->getInterface().SetRestitution(sphere.getComponent<Components::Rigidbody3DComponent>().physicsBody, 0.4f);
	}
}