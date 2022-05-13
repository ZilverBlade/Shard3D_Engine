#pragma once

#include "shard_window.hpp"
#include "shard_pipeline.hpp"
#include "shard_device.hpp"
#include "camera.hpp"
#include "shard_frame_info.hpp"

#include "shard_game_object.hpp"
#include <string>
#include "shard_renderer.hpp"

#include <memory>
#include <vector>

namespace shard {
	class EzRenderSystem {
	public:
		EzRenderSystem(ShardDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~EzRenderSystem();

		EzRenderSystem(const EzRenderSystem&) = delete;
		EzRenderSystem& operator=(const EzRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo, std::vector<ShardGameObject> &gameObjects);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		ShardDevice& shardDevice;

		std::unique_ptr<ShardPipeline> shardPipeline;
		VkPipelineLayout pipelineLayout;
	};

}