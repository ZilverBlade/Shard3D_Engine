#pragma once

#include "shard_window.hpp"
#include "shard_device.hpp"
#include <cassert>
#include <string>
#include "shard_swap_chain.hpp"

#include <memory>
#include <vector>

namespace shard {
	class ShardRenderer {
	public:

		ShardRenderer(ShardWindow &window, ShardDevice &device);
		~ShardRenderer();

		ShardRenderer(const ShardRenderer &) = delete;
		ShardRenderer &operator=(const ShardRenderer &) = delete;

		VkRenderPass getSwapChainRenderPass() const { return shardSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex]; 
		}

		int getFrameIndex()const {
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapchain();
		
		ShardWindow &shardWindow;
		ShardDevice &shardDevice;
		std::unique_ptr<ShardSwapChain> shardSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;	

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted{false};
	};
}