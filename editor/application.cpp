#include "application.h"

#include <thread>
#include <fstream>
#include <vector>
#include <Shard3D/systems/systems.h>

#include <Shard3D/core/asset/assetmgr.h>
#include <Shard3D/core/ui/hud_layer.h>

#include "camera/editor_movement_controller.h"
#include "imgui/imgui_layer.h"
#include "imgui/imgui_initializer.h"

#include <Shard3D/core/misc/graphics_settings.h>

#include <Shard3D/scripting/script_engine.h>
#include <Shard3D/scripting/script_handler.h>

#include <Shard3D/workarounds.h>

// c++ scripts
#include "scripting/script_link.h"

namespace Shard3D {
	EditorApplication::EditorApplication() {
		createRenderPasses();
		setupEngineFeatures();

		SHARD3D_INFO("Constructing Level Pointer");
		level = make_sPtr<ECS::Level>();
	}
	EditorApplication::~EditorApplication() {
	
	}

	void EditorApplication::createRenderPasses() {
		{ // Main renderer
			mainColorFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				GraphicsSettings::get().MSAASamples
				}, FrameBufferAttachmentType::Color);

			mainDepthFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				engineRenderer.getSwapchain()->findDepthFormat(),
				VK_IMAGE_ASPECT_DEPTH_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
				GraphicsSettings::get().MSAASamples
				}, FrameBufferAttachmentType::Depth);


			mainResolveFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_SAMPLE_COUNT_1_BIT
				}, FrameBufferAttachmentType::Resolve);

			AttachmentInfo colorAttachmentInfo{};
			colorAttachmentInfo.frameBufferAttachment = mainColorFramebufferAttachment;
			colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			AttachmentInfo depthAttachmentInfo{};
			depthAttachmentInfo.frameBufferAttachment = mainDepthFramebufferAttachment;
			depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			AttachmentInfo resolveAttachmentInfo{};
			resolveAttachmentInfo.frameBufferAttachment = mainResolveFramebufferAttachment;
			resolveAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			resolveAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			
			mainRenderpass = new RenderPass(
				engineDevice, {
				colorAttachmentInfo,
				depthAttachmentInfo,
				resolveAttachmentInfo
				//depthresolveAttachmentInfo
				});

			mainFrameBuffer = new FrameBuffer(engineDevice, mainRenderpass->getRenderPass(), { mainColorFramebufferAttachment, mainDepthFramebufferAttachment, mainResolveFramebufferAttachment });
		}
		{ // Post processing
			ppoColorFramebufferAttachment = new FrameBufferAttachment(engineDevice, {
			VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				glm::ivec3(1920, 1080, 1),
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

	void EditorApplication::destroyRenderPasses() {
		delete mainFrameBuffer;
		delete mainRenderpass;
		delete mainColorFramebufferAttachment;
		delete mainDepthFramebufferAttachment;
		delete mainResolveFramebufferAttachment;

		delete ppoFrameBuffer;
		delete ppoColorFramebufferAttachment;
		delete ppoRenderpass;
	}

	void EditorApplication::setupEngineFeatures() {
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

	void EditorApplication::setWindowCallbacks() {
		SHARD3D_EVENT_BIND_HANDLER_RFC(engineWindow, EditorApplication::eventEvent);
	}

	void EditorApplication::windowResizeEvent(Events::WindowResizeEvent& e) {
		createRenderPasses();
	}

	void EditorApplication::eventEvent(Events::Event& e) {
	}

	void EditorApplication::run() {
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
		PostProcessingSystem ppoSystem { engineDevice, ppoRenderpass->getRenderPass(), {mainResolveFramebufferAttachment, nullptr, nullptr, nullptr}};
		ShadowMappingSystem shadowSystem { engineDevice };

		BillboardRenderSystem billboardRenderSystem { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		_EditorBillboardRenderer editorBillboardRenderer { engineDevice, mainRenderpass->getRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PhysicsSystem physicsSystem{};
		LightSystem lightSystem{};

		level->setPhysicsSystem(&physicsSystem);

		ResourceHandler::init(engineDevice);

		{
			CSimpleIniA ini;
			ini.SetUnicode();
			ini.LoadFile(EDITOR_SETTINGS_PATH);

			ImGuiInitializer::init(engineDevice, engineWindow, engineRenderer.getSwapChainRenderPass(), ini.GetBoolValue("THEME", "useLightMode"));
		}
		ImGuiLayer* imguiLayer = new ImGuiLayer();

		ImGuiInitializer::setViewportImage(&imguiLayer->viewportImage, ppoColorFramebufferAttachment);

		layerStack.pushOverlay(imguiLayer);

		// TODO: render the HUDLayer to a seperate renderpass, then render that over the mainoffscreen in the editor viewport, 
		// but render the HUD seperately from everything in the GUIEditor window.
		HUDLayer* hudLayer0 = new HUDLayer();
		HUDLayer* hudLayer1 = new HUDLayer();
		HUDLayer* hudLayer2 = new HUDLayer();
		HUDLayer* hudLayer3 = new HUDLayer();
		hudLayer0->layer = 0;
		hudLayer1->layer = 1;
		hudLayer2->layer = 2;
		hudLayer3->layer = 3;
		layerStack.pushOverlay(hudLayer3);
		layerStack.pushOverlay(hudLayer2);
		layerStack.pushOverlay(hudLayer1);
		layerStack.pushOverlay(hudLayer0);

		SHARD3D_INFO("Loading editor camera actor");
		ECS::Actor editor_cameraActor = level->createActorWithUUID(0, "Editor Camera Actor (SYSTEM RESERVED)");
		editor_cameraActor.addComponent<Components::CameraComponent>();
		editor_cameraActor.getComponent<Components::CameraComponent>().postProcessMaterials.emplace_back(ResourceHandler::retrievePPOMaterial(AssetID("assets/_engine/mat/ppo/hdr_vfx.s3dasset")).get(), AssetID("assets/_engine/mat/ppo/hdr_vfx.s3dasset"));
		editor_cameraActor.getComponent<Components::CameraComponent>().postProcessMaterials.emplace_back(ResourceHandler::retrievePPOMaterial(AssetID("assets/_engine/mat/ppo/bloom_vfx.s3dasset")).get(), AssetID("assets/_engine/mat/ppo/bloom_vfx.s3dasset"));
		
		level->setPossessedCameraActor(editor_cameraActor);
		editor_cameraActor.getComponent<Components::TransformComponent>().setTranslation(glm::vec3(0.f, -1.f, 1.f));
		SHARD3D_INFO("Loading dummy actor");
		ECS::Actor dummy = level->createActorWithUUID(1, "Dummy Actor (SYSTEM RESERVED)");

		controller::EditorMovementController editorCameraController{};
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
		loadStaticObjects();

		HUDLayer* layerList[4]{
			hudLayer0,
			hudLayer1,
			hudLayer2,
			hudLayer3
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
		imguiLayer->attachGUIEditorInfo(h_l_layer);

		auto currentTime = std::chrono::high_resolution_clock::now();
beginWhileLoop:
		while (!engineWindow.shouldClose()) {
			StatsTimer::dumpIf();
			StatsTimer::clear();
			SHARD3D_STAT_RECORD();
			glfwPollEvents();
			SHARD3D_STAT_RECORD_END({"Window", "Polling"});
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			auto possessedCameraActor = level->getPossessedCameraActor();
			auto& possessedCamera = level->getPossessedCamera();
			editor_cameraActor = level->getActorFromUUID(0);

			if (level->simulationState == PlayState::Playing) {
				SHARD3D_STAT_RECORD(); 
				physicsSystem.simulate(level.get(), frameTime);
				SHARD3D_STAT_RECORD_END({ "Level", "Physics" });
				SHARD3D_STAT_RECORD();
				level->tick(frameTime);
				SHARD3D_STAT_RECORD_END({ "Level", "Tick" });
			}
			SHARD3D_STAT_RECORD();
			level->runGarbageCollector(engineDevice);
			ResourceHandler::runGarbageCollector();
			ECS::MasterManager::executeQueue(level, engineDevice);
			EngineAudio::globalUpdate(possessedCameraActor.getTransform().getTranslation(), 
				possessedCameraActor.getTransform().getRotation());
			SHARD3D_STAT_RECORD_END({ "Engine", "Garbage Collection" });
			if (level->simulationState != PlayState::Playing) {
				editorCameraController.tryPoll(engineWindow, frameTime, editor_cameraActor);
			}
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
					this,
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

				//ubo.cameraSettings = { GraphicsSettings::get().GlobalMaterialSettings,  GraphicsSettings::get().exposure };

				lightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				SHARD3D_STAT_RECORD_END({"UBO", "Update"});
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
				SHARD3D_STAT_RECORD();
				shadowSystem.render(frameInfo);
				SHARD3D_STAT_RECORD_END({ "Rendering", "Shadow Mapping" });

				mainRenderpass->beginRenderPass(frameInfo, mainFrameBuffer);
				SHARD3D_STAT_RECORD();
				forwardRenderSystem.renderForward(frameInfo);
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
				ppoRenderpass->endRenderPass(frameInfo);
				SHARD3D_STAT_RECORD_END({ "Post Processing", "Debanding" });
				engineRenderer.beginSwapChainRenderPass(commandBuffer);
				// Layer overlays (use UI here)
				for (Layer* layer : layerStack) {
					layer->update(frameInfo);
				}
				SHARD3D_STAT_RECORD();
				engineRenderer.endSwapChainRenderPass(commandBuffer);
				SHARD3D_STAT_RECORD_END({ "Swapchain", "End" });
				// Command buffer ends
				SHARD3D_STAT_RECORD();
				engineRenderer.endFrame();
				SHARD3D_STAT_RECORD_END({ "Command Buffer", "Submit" });
			}
		}

		if (MessageDialogs::show("Any unsaved changes will be lost! Are you sure you want to exit?",
			"Shard3D Torque", MessageDialogs::OPTYESNO | MessageDialogs::OPTDEFBUTTON2 | MessageDialogs::OPTICONEXCLAMATION) == MessageDialogs::RESNO) {
			glfwSetWindowShouldClose(engineWindow.getGLFWwindow(), GLFW_FALSE);
			goto beginWhileLoop;
		}

		if (level->simulationState != PlayState::Stopped) level->end();

		ShaderSystem::destroy();
		ScriptEngine::destroy();
		vkDeviceWaitIdle(engineDevice.device());

		ResourceHandler::destroy();
		_special_assets::_editor_icons_destroy();
		
		for (Layer* layer : layerStack) {
			layer->detach();
		}
		destroyRenderPasses();
		SharedPools::destructPools();

		level = nullptr;
	}

	void EditorApplication::loadStaticObjects() {
		
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