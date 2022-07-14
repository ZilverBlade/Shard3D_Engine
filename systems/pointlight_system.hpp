#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"

#include "../components.hpp"
#include "../renderer.hpp"

namespace Shard3D {
	class PointlightSystem {
	public:
		PointlightSystem(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointlightSystem();

		PointlightSystem(const PointlightSystem&) = delete;
		PointlightSystem& operator=(const PointlightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo &ubo, std::shared_ptr<wb3d::Level>& level);
		void render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Level>& level);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;
		std::unique_ptr<EngineDescriptorSetLayout> billboardSystemLayout;
		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};

}