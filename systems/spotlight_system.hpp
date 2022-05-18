#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"
		  
#include "../game_object.hpp"
#include <string>
#include "../renderer.hpp"

#include <memory>
#include <vector>

namespace Shard3D {
	class SpotlightSystem {
	public:
		SpotlightSystem(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SpotlightSystem();

		SpotlightSystem(const SpotlightSystem&) = delete;
		SpotlightSystem& operator=(const SpotlightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo &ubo);
		void render(FrameInfo &frameInfo);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};

}