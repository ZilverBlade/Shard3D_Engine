#pragma once

#include "window.h"
#include "../vulkan_api/device.h"
#include "swap_chain.h"

namespace Shard3D {
	class EngineRenderer {
	public:
		EngineRenderer(EngineWindow &window, EngineDevice &device);
		~EngineRenderer();

		EngineRenderer(const EngineRenderer &) = delete;
		EngineRenderer &operator=(const EngineRenderer &) = delete;

		VkRenderPass getSwapChainRenderPass() const { return engineSwapChain->getRenderPass(); }
		float getAspectRatio() const { return engineSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			SHARD3D_ASSERT(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex]; 
		}

		VkImageView getSwapchainImageView(int index) { return engineSwapChain->getImageView(index); }

		int getFrameIndex()const {
			SHARD3D_ASSERT(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame(std::chrono::time_point<std::chrono::steady_clock>& beginTimePoint);
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
		uPtr<EngineSwapChain>& getSwapchain() { return engineSwapChain; }
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapchain();
		
		EngineWindow &engineWindow;
		EngineDevice &engineDevice;
		uPtr<EngineSwapChain> engineSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;	

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted{false};

		float noEditBgColor[3] = { 0.01f, 0.01f, 0.01f };
		std::array<VkClearValue, 2> clearValues{};
	};
}