#pragma once
#include "../s3dtpch.h"
#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

#include "../renderer.hpp"


namespace Shard3D {
	class ComputeSystem {
	public:
		ComputeSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ComputeSystem();

		ComputeSystem(const ComputeSystem&) = delete;
		ComputeSystem& operator=(const ComputeSystem&) = delete;

		void render(FrameInfo& frameInfo);
	private:

		void createPipeline(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		std::unique_ptr<EngineDescriptorSetLayout> renderSystemLayout;
	};

}