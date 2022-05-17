#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "run_app.hpp"
//lib stuff
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <array>

//glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//engine
#include "input/keyboard_movement_controller.hpp"
#include "input/mouse_movement_controller.hpp"
#include "camera.hpp"
#include "utils/shard_utils.hpp"
#include "utils/definitions.hpp"

#include "simpleini/simple_ini.h"
#include "shard_buffer.hpp"

//systems
#include "systems/ez_render_system.hpp"

#include "systems/pointlight_system.hpp"
#include "systems/spotlight_system.hpp"
#include "systems/directional_light_system.hpp"


namespace shard {

	RunApp::RunApp() {
		std::ifstream infile(ENGINE_SETTINGS_PATH);
		assert(infile.good() != false && "Critical error! Engine settings config file not found!");
		std::ifstream infile2(GAME_SETTINGS_PATH);
		assert(infile2.good() != false && "Critical error! Game settings config file not found!");
	
		globalPool = ShardDescriptorPool::Builder(shardDevice)
			.setMaxSets(ShardSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ShardSwapChain::MAX_FRAMES_IN_FLIGHT)			
			.build();
		loadGameObjects();
	}
	RunApp::~RunApp() {}
	void RunApp::run() {
		std::vector<std::unique_ptr<ShardBuffer>> uboBuffers(ShardSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<ShardBuffer>(
				shardDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = ShardDescriptorSetLayout::Builder(shardDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(ShardSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			ShardDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		EzRenderSystem ezRenderSystem{ shardDevice, shardRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointlightSystem pointlightSystem { shardDevice, shardRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		SpotlightSystem spotlightSystem{ shardDevice, shardRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		DirectionalLightSystem directionalLightSystem{ shardDevice, shardRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		ShardCamera camera{};

		auto viewerObject = ShardGameObject::createGameObject();
		viewerObject.transform.translation = glm::vec3(0.f, -1.f, -1.f);

		controller::KeyboardMovementController cameraControllerKeyBoard{};
		controller::MouseMovementController cameraControllerMouse{};

		double mousePosX = {};
		double mousePosY = {};

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		float fov = ini.GetDoubleValue("RENDERING", "FOV");
		std::cout << "Default FOV set to " << fov << " degrees" << std::endl;

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!shardWindow.shouldClose()) {

			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			glfwGetCursorPos(shardWindow.getGLFWwindow(), &mousePosX, &mousePosY);

			cameraControllerKeyBoard.moveInPlaneXZ(shardWindow.getGLFWwindow(), frameTime, viewerObject);
			cameraControllerMouse.moveInPlaneXZ(shardWindow.getGLFWwindow(), frameTime, viewerObject, { mousePosX, mousePosY });
			
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = shardRenderer.getAspectRatio();
			
			if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") {
				camera.setPerspectiveProjection(glm::radians(fov), aspect, ini.GetDoubleValue("RENDERING", "NearClipDistance"), ini.GetDoubleValue("RENDERING", "FarClipDistance"));
			}
			else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic"){
				camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, ini.GetDoubleValue("RENDERING", "FarClipDistance"));  //Ortho perspective (not needed 99.99% of the time)
			}
				
			if (auto commandBuffer = shardRenderer.beginFrame()) {
				int frameIndex = shardRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				//	update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();

				pointlightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				spotlightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				directionalLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//	render
				/*
					this section is great for adding multiple render passes such as :
					- Begin offscreen shadow pass
					- render shadow casting objects
					- end offscreen shadow pass

					Also reflections and Postfx
				*/
				shardRenderer.beginSwapChainRenderPass(commandBuffer); 

				ezRenderSystem.renderGameObjects(frameInfo);
				pointlightSystem.render(frameInfo);
				spotlightSystem.render(frameInfo);
				directionalLightSystem.render(frameInfo);

				shardRenderer.endSwapChainRenderPass(commandBuffer);
				shardRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(shardDevice.device());
		
	}

	void RunApp::loadGameObjects() {	
		std::shared_ptr<ShardModel> model = ShardModel::createModelFromFile(shardDevice, "modeldata/FART.obj", false); //dont index because model breaks

		auto fart = ShardGameObject::createGameObject();
		fart.model = model;
		fart.transform.translation = { .0f, .0f, 1.5f };
		fart.transform.scale = { .5f, .5f, .5f };
		fart.transform.rotation = { glm::radians(90.f), 0.f, 0.f};
		gameObjects.emplace(fart.getId(), std::move(fart));

		auto fart2 = ShardGameObject::createGameObject();
		fart2.model = model;
		fart2.transform.translation = { 0.0f, .0f, 5.5f };
		fart2.transform.scale = { .5f, .5f, .5f };
		fart2.transform.rotation = { glm::radians(90.f), glm::radians(90.f), 0.f };
		gameObjects.emplace(fart2.getId(), std::move(fart2));

		model = ShardModel::createModelFromFile(shardDevice, "modeldata/colored_cube.obj");

		auto ccube = ShardGameObject::createGameObject();
		ccube.model = model;
		ccube.transform.translation = { 2.0f, .0f, 1.5f };
		ccube.transform.scale = { .5f, .5f, .5f };
		ccube.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(ccube.getId(), std::move(ccube));

		model = ShardModel::createModelFromFile(shardDevice, "modeldata/sphere.obj", false);

		auto sphere = ShardGameObject::createGameObject();
		sphere.model = model;
		sphere.transform.translation = { 3.0f, .0f, 5.5f };
		sphere.transform.scale = { 1.f, 1.f, 1.f };
		sphere.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(sphere.getId(), std::move(sphere));

		model = ShardModel::createModelFromFile(shardDevice, "modeldata/cylinder.obj");

		auto cylinder = ShardGameObject::createGameObject();
		cylinder.model = model;
		cylinder.transform.translation = { 5.0f, .0f, 5.5f };
		cylinder.transform.scale = { .5f, .5f, .5f };
		cylinder.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(cylinder.getId(), std::move(cylinder));

		model = ShardModel::createModelFromFile(shardDevice, "modeldata/quad.obj");

		auto quad = ShardGameObject::createGameObject();
		quad.model = model;
		quad.transform.translation = { 0.0f, 0.9f, 0.0f };
		quad.transform.scale = { 10.f, 1.f, 10.f };
		quad.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(quad.getId(), std::move(quad));

		model = ShardModel::createModelFromFile(shardDevice, "modeldata/cone.obj");

		auto cone = ShardGameObject::createGameObject();
		cone.model = model;
		cone.transform.translation = { 1.0f, -1.0f, 6.0f };
		cone.transform.scale = { 0.5f, 0.5f, 0.5f };
		cone.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(cone.getId(), std::move(cone));

		auto cone2 = ShardGameObject::createGameObject();
		cone2.model = model;
		cone2.transform.translation = { .0f, -1.0f, 6.0f };
		cone2.transform.scale = { 0.5f, 0.5f, 0.5f };
		cone2.transform.rotation = { 0.f, 0.f, 0.f };
		gameObjects.emplace(cone2.getId(), std::move(cone2));
		/*
		{
			auto pointlight = ShardGameObject::makePointlight(1.f);
			pointlight.transform.translation = { 2.0f, -1.0f, 2.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}

		{
			auto pointlight = ShardGameObject::makePointlight(1.f);
			pointlight.transform.translation = { -2.0f, -1.0f, 2.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}

		{
			auto pointlight = ShardGameObject::makePointlight(1.f);
			pointlight.transform.translation = { 0.0f, -1.0f, 0.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}


		{
			auto pointlight = ShardGameObject::makePointlight(1.f);
			pointlight.transform.translation = { 0.0f, -1.0f, 3.0f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}
	
		{
			auto pointlight = ShardGameObject::makePointlight(0.3f, 0.1, {1.f, 0.f, 1.f});
			pointlight.transform.translation = { 0.0f, -0.2f, 0.2f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}
		*/
		{
			auto pointlight = ShardGameObject::makePointlight(0.5f, 0.1, { 1.f, 1.f, 0.f }, { 0.0f, -0.2f, 1.3f });
			pointlight.transform.translation = { 1.0f, -0.2f, 0.2f };
			gameObjects.emplace(pointlight.getId(), std::move(pointlight));
		}
		
		{
			auto spotlight = ShardGameObject::makeSpotlight(1.3f, 3.1, { 1.f, 1.f, 1.f }, { 0.f, glm::radians(90.f), 0.f }, glm::radians(30.f), glm::radians(40.f), {0.f, 0.f, 1.f});
			spotlight.transform.translation = { 4.0f, -0.1f, 0.2f };
			gameObjects.emplace(spotlight.getId(), std::move(spotlight));
		}
		{
			auto spotlight = ShardGameObject::makeSpotlight(0.2f, 3.1, { 1.f, 1.f, 1.f }, { 0.f, glm::radians(60.f), 1.f }, glm::radians(20.f), glm::radians(50.f), { 0.f, 0.f, 1.f });
			spotlight.transform.translation = { 6.0f, -0.1f, 0.2f };
			gameObjects.emplace(spotlight.getId(), std::move(spotlight));
		}
		
		{
			auto directionalLight = ShardGameObject::makeDirectionalLight(0.01f, glm::vec3(1.f, 1.0f, 1.f));
			directionalLight.transform.translation = { 2.0f, -0.5f, 0.2f };
			gameObjects.emplace(directionalLight.getId(), std::move(directionalLight));
		}

	}
}