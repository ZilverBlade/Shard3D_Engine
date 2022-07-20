#pragma once

#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"

#include "../components.hpp"
#include "../renderer.hpp"

namespace Shard3D {
	class _EditorBillboardRenderer {
	public:
		_EditorBillboardRenderer(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~_EditorBillboardRenderer();

		_EditorBillboardRenderer(const _EditorBillboardRenderer&) = delete;
		_EditorBillboardRenderer& operator=(const _EditorBillboardRenderer&) = delete;

		void render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Level>& level);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		template <typename T>
		void renderComponent(const char* iconName, FrameInfo& frameInfo, std::shared_ptr<wb3d::Level>& level);

		EngineDevice& engineDevice;
		std::unique_ptr<EngineDescriptorSetLayout> billboardSystemLayout;
		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};
}