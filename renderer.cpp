#include "renderer.hpp"
#include "run_app.hpp"
#include "utils/definitions.hpp"
#include "simpleini/simple_ini.h"

#include <cassert>
#include <stdexcept>
#include <array>

namespace Shard3D {

	EngineRenderer::EngineRenderer(EngineWindow &window, EngineDevice &device) 
		: engineWindow{window}, engineDevice{device} {
		recreateSwapchain();
		createCommandBuffers();
	}
	EngineRenderer::~EngineRenderer() {
		freeCommandBuffers();
	}
	

	void EngineRenderer::recreateSwapchain() {
		auto extent = engineWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = engineWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(engineDevice.device());
		engineSwapChain = nullptr; //for some reason validation fails when new swap chain is created
		if (engineSwapChain == nullptr) {
			engineSwapChain = std::make_unique<EngineSwapChain>(engineDevice, extent);
		}
		else {
			std::shared_ptr<EngineSwapChain> oldSwapChain = std::move(engineSwapChain);
			engineSwapChain = std::make_unique<EngineSwapChain>(engineDevice, extent, std::move(engineSwapChain));

			if (!oldSwapChain->compareSwapFormats(*engineSwapChain.get())) {
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}
	}

	void EngineRenderer::createCommandBuffers() {
		commandBuffers.resize(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = engineDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(engineDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void EngineRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			engineDevice.device(),
			engineDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	
	VkCommandBuffer EngineRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");

		auto result = engineSwapChain->acquireNextImage(&currentImageIndex);

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
	void EngineRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = engineSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || engineWindow.wasWindowResized()) {
			engineWindow.resetWindowResizedFlag();
			recreateSwapchain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % EngineSwapChain::MAX_FRAMES_IN_FLIGHT;
	}
	void EngineRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = engineSwapChain->getRenderPass();
		renderPassInfo.framebuffer = engineSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = engineSwapChain->getSwapChainExtent();

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
		viewport.width = (float)(engineSwapChain->getSwapChainExtent().width);
		viewport.height = (float)(engineSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, engineSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void EngineRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}



}