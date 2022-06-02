#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

#include "../wb3d/scene.hpp"
#include "../wb3d/actor.hpp"

#include "../components.hpp"
#include <string>
#include "../renderer.hpp"

#include <memory>
#include <vector>

namespace Shard3D {
	class PointlightSystem {
	public:
		PointlightSystem(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointlightSystem();

		PointlightSystem(const PointlightSystem&) = delete;
		PointlightSystem& operator=(const PointlightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo &ubo, std::shared_ptr<wb3d::Scene>& scene);
		void render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Scene>& scene);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};

}