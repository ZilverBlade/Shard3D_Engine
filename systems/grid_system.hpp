#pragma once
#include "../s3dtpch.h"
#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

#include "../renderer.hpp"

namespace Shard3D {
	class GridSystem {
	public:
		GridSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~GridSystem();

		GridSystem(const GridSystem&) = delete;
		GridSystem& operator=(const GridSystem&) = delete;

		void render(FrameInfo& frameInfo);
		void recreatePipeline(VkRenderPass renderPass);
	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);


		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};

}