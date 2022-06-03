#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"
		  

#include "../components.hpp"
#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"

#include <string>
#include "../renderer.hpp"

#include <memory>
#include <vector>

namespace Shard3D {
	class BasicRenderSystem {
	public:
		BasicRenderSystem(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~BasicRenderSystem();

		BasicRenderSystem(const BasicRenderSystem&) = delete;
		BasicRenderSystem& operator=(const BasicRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo, std::shared_ptr<wb3d::Level>& level);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};

}