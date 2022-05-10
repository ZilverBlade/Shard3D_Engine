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

	// temporary helper function, creates a 1x1x1 cube centered at offset
	std::unique_ptr<ShardModel> createCubeModel(ShardDevice& device, glm::vec3 offset) {
		std::vector<ShardModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<ShardModel>(device, vertices);
	}


	void RunApp::loadGameObjects() {	
		std::shared_ptr<ShardModel> shardModel = createCubeModel(shardDevice, { .0f, .0f, .0f });

		auto cube = ShardGameObject::createGameObject();
		cube.model = shardModel;
		cube.transform.translation = { .0f, .0f, .5f };
		cube.transform.scale = { .5f, .5f, .5f };

		gameObjects.push_back(std::move(cube));
	}
}