#pragma once
#include "../s3dtpch.h"
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
	class BillboardRenderSystem {
	public:
		BillboardRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~BillboardRenderSystem();

		BillboardRenderSystem(const BillboardRenderSystem&) = delete;
		BillboardRenderSystem& operator=(const BillboardRenderSystem&) = delete;

		void render(FrameInfo& frameInfo, std::shared_ptr<wb3d::Level>& level);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		std::unique_ptr<EngineDescriptorSetLayout> billboardSystemLayout;
	};

}