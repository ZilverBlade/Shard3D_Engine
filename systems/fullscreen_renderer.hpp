#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

#include "../renderer.hpp"

namespace Shard3D {
	class FullscreenRenderer {
	public:
		FullscreenRenderer(VkRenderPass renderPass);
		~FullscreenRenderer();

		FullscreenRenderer(const FullscreenRenderer&) = delete;
		FullscreenRenderer& operator=(const FullscreenRenderer&) = delete;

		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		std::unique_ptr<EngineDescriptorSetLayout> quadSystemLayout;
		VkDescriptorSet screenImageDescriptorSet;
		VkDescriptorImageInfo imageInfo{};
	};

}