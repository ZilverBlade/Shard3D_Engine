#pragma once

#include "../../../s3dstd.h"
#include "../../../vulkan_abstr.h"
#include "../../../core/ecs/level.h"

namespace Shard3D {
	class _EditorBillboardRenderer {
	public:
		_EditorBillboardRenderer(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~_EditorBillboardRenderer();

		_EditorBillboardRenderer(const _EditorBillboardRenderer&) = delete;
		_EditorBillboardRenderer& operator=(const _EditorBillboardRenderer&) = delete;

		void render(FrameInfo &frameInfo);
	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		template <typename T>
		void renderComponent(const char* iconName, FrameInfo& frameInfo);

		EngineDevice& engineDevice;
		uPtr<EngineDescriptorSetLayout> billboardSystemLayout;
		uPtr<GraphicsPipeline> graphicsPipeline;
		VkPipelineLayout pipelineLayout;
	};
}