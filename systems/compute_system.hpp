#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

#include <string>
#include "../renderer.hpp"

#include <memory>
#include <vector>

namespace Shard3D {
	class ComputeSystem {
	public:
		ComputeSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ComputeSystem();

		ComputeSystem(const ComputeSystem&) = delete;
		ComputeSystem& operator=(const ComputeSystem&) = delete;

		void render(FrameInfo& frameInfo);
	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};

}