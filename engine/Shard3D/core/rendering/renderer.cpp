#include "../../s3dpch.h"  
#include "renderer.h"

#include "../../core.h"
#include "../misc/graphics_settings.h"

namespace Shard3D {
	EngineRenderer::EngineRenderer(EngineWindow &window, EngineDevice &device) 
		: engineWindow{window}, engineDevice{device} {
		recreateSwapchain();
		createCommandBuffers();

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		noEditBgColor[0] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorR"));
		noEditBgColor[1] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorG"));
		noEditBgColor[2] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorB"));
	
		clearValues[0].color = { noEditBgColor[0], noEditBgColor[1], noEditBgColor[2], 1.f };

		clearValues[1].depthStencil = { 1.0f, 0 };
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
		engineSwapChain = nullptr;
		engineSwapChain = make_uPtr<EngineSwapChain>(engineDevice, extent);		
		GraphicsSettings::r_info.aspectRatio = getAspectRatio();
	}

	void EngineRenderer::createCommandBuffers() {
		commandBuffers.resize(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = engineDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(engineDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to allocate command buffers!");
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
		SHARD3D_ASSERT(!isFrameStarted && "Can't call beginFrame while already in progress");

		auto result = engineSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			SHARD3D_FATAL("failed to acquire swap chain image!");
		}

		isFrameStarted = true;
		
		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}
	void EngineRenderer::endFrame(std::chrono::time_point<std::chrono::steady_clock>& beginTimePoint) {
		SHARD3D_ASSERT(isFrameStarted && "Can't call endFrame while frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			SHARD3D_FATAL("failed to record command buffer!");
		}

		const float frameTime = std::chrono::duration<double, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - beginTimePoint).count();
		const float waitTime = std::max(GraphicsSettings::get().FramerateCapInterval - frameTime, 0.f);

		auto result = engineSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex, waitTime);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
		} else if (result != VK_SUCCESS) {
			SHARD3D_FATAL("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % EngineSwapChain::MAX_FRAMES_IN_FLIGHT;
	}
	void EngineRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		SHARD3D_ASSERT(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		SHARD3D_ASSERT(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = engineSwapChain->getRenderPass();
		renderPassInfo.framebuffer = engineSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = engineSwapChain->getSwapChainExtent();

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(engineSwapChain->getSwapChainExtent().height);
		viewport.width = static_cast<float>(engineSwapChain->getSwapChainExtent().width);
		viewport.height = -static_cast<float>(engineSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, engineSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}
	void EngineRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		SHARD3D_ASSERT(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		SHARD3D_ASSERT(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}