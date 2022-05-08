#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "ez_render_system.hpp"
#include "run_app.hpp"
#include <stdexcept>
#include <array>
#include "FMOD_engine/fmod_core_engine.hpp"
#include "FMOD_engine/fmod_studio_engine.hpp"


namespace shard {

	RunApp::RunApp() {
		loadGameObjects();
	}
	RunApp::~RunApp() {}
	void RunApp::run() {
		EzRenderSystem ezRenderSystem{ shardDevice, shardRenderer.getSwapChainRenderPass() };

		/*
			just some fmod stuff bcos why not
		*/

		fmod_engine::FMOD_Core_Engine fmodcore;
		fmod_engine::FMOD_Studio_Engine fmodstudio;

		fmodcore.PlaySoundFile("sounddata/9kkkkkkkkkk.wav"); //zyn :)
		fmodstudio.PlayBankEvent("sounddata/FMOD/Desktop", "engines.bank", "event:/carsounds/arrive");
		fmodstudio.PlayBankEvent("sounddata/FMOD/Desktop", "engines.bank", "event:/carsounds/idlerace", "car_level", 0);

		fmodcore.UpdateVolume(0.75);
		fmodcore.UpdatePitch(0.9);

		while (!shardWindow.shouldClose()) {
			glfwPollEvents();
			if (auto commandBuffer = shardRenderer.beginFrame()) {
				/*
					this section is great for adding multiple render passes such as :
					- Begin offscreen shadow pass
					- render shadow casting objects
					- end offscreen shadow pass

					Also reflections and Postfx
				*/
				shardRenderer.beginSwapChainRenderPass(commandBuffer); 
				ezRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				shardRenderer.endSwapChainRenderPass(commandBuffer);
				shardRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(shardDevice.device());
	}

	void RunApp::loadGameObjects() {	
		std::vector<ShardModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1, 0, 1}},
			{{0.5f, 0.5f}, {0, 1, 1}},
			{{-0.5f, 0.5f}, {1, 1, 0}}
		};
	   auto	shardModel = std::make_shared<ShardModel>(shardDevice, vertices);

	   auto triangle = ShardGameObject::createGameObject();
	   triangle.model = shardModel;
	   triangle.color = { 1.0f, 0.4f, 0.8f };
	   triangle.transform2d.translation.x = .2f;
	   triangle.transform2d.scale = { 2.f, 0.5f };
	   triangle.transform2d.rotation = .25f * glm::two_pi<float>();

	   gameObjects.push_back(std::move(triangle));
	}
}