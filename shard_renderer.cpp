#include "shard_renderer.hpp"
#include "run_app.hpp"
#include "utils/definitions.hpp"
#include "simpleini/simple_ini.h"

#include <cassert>
#include <stdexcept>
#include <array>

namespace shard {

	ShardRenderer::ShardRenderer(ShardWindow &window, ShardDevice &device) 
		: shardWindow{window}, shardDevice{device} {
		recreateSwapchain();
		createCommandBuffers();
	}
	ShardRenderer::~ShardRenderer() {
		freeCommandBuffers();
	}
	

	void ShardRenderer::recreateSwapchain() {
		auto extent = shardWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = shardWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(shardDevice.device());
		shardSwapChain = nullptr; //for some reason validation fails when new swap chain is created
		if (shardSwapChain == nullptr) {
			shardSwapChain = std::make_unique<ShardSwapChain>(shardDevice, extent);
		}
		else {
			std::shared_ptr<ShardSwapChain> oldSwapChain = std::move(shardSwapChain);
			shardSwapChain = std::make_unique<ShardSwapChain>(shardDevice, extent, std::move(shardSwapChain));

			if (!oldSwapChain->compareSwapFormats(*shardSwapChain.get())) {
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}
		//come back later
	}

	void ShardRenderer::createCommandBuffers() {
		commandBuffers.resize(ShardSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = shardDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(shardDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void ShardRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			shardDevice.device(),
			shardDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	
	VkCommandBuffer ShardRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");

		auto result = shardSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}
	void ShardRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = shardSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || shardWindow.wasWindowResized()) {
			shardWindow.resetWindowResizedFlag();
			recreateSwapchain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % ShardSwapChain::MAX_FRAMES_IN_FLIGHT;
	}
	void ShardRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = shardSwapChain->getRenderPass();
		renderPassInfo.framebuffer = shardSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = shardSwapChain->getSwapChainExtent();

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 
			(float)ini.GetDoubleValue("RENDERING", "DefaultBGColorR"), 
			(float)ini.GetDoubleValue("RENDERING", "DefaultBGColorG"),
			(float)ini.GetDoubleValue("RENDERING", "DefaultBGColorB"),
			1.0f
		};

		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(shardSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(shardSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, shardSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void ShardRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}



}