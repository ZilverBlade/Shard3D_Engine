#pragma once

#include "../vulkan_api/device.h"
#include "frame_buffer.h"
#include "../misc/frame_info.h"
#include "../../s3dstd.h"

namespace Shard3D {
	struct AttachmentInfo {
		FrameBufferAttachment* frameBufferAttachment;
		VkAttachmentLoadOp loadOp;
		VkAttachmentStoreOp storeOp;
	};

	struct SubpassInfo {
		std::vector<uint32_t> renderTargets;
		std::vector<uint32_t> subpassInputs;
	};

	struct SubpassDescription {
		VkAttachmentReference depthReference{};
		std::vector<VkAttachmentReference> colorReferences{};
		std::vector<VkAttachmentReference> inputReferences{};
		VkSubpassDescription subpassDescription;
	};

	
	class SimpleRenderPass {
	public:
		SimpleRenderPass(EngineDevice& device, const std::vector<AttachmentInfo>& attachments);
		~SimpleRenderPass();

		SimpleRenderPass(const SimpleRenderPass&) = delete;
		SimpleRenderPass& operator=(const SimpleRenderPass&) = delete;
		SimpleRenderPass(SimpleRenderPass&&) = delete;
		SimpleRenderPass& operator=(SimpleRenderPass&&) = delete;

		void beginRenderPass(FrameInfo& frameInfo, FrameBuffer* frameBuffer);
		void endRenderPass(FrameInfo& frameInfo);

		inline void setViewportSize(uint32_t _width, uint32_t _height) { width = _width; height = _height; }

		inline VkRenderPass getRenderPass() { return renderpass; }
	private:
		uint32_t width{};
		uint32_t height{};

		EngineDevice& engineDevice;

		std::vector<VkClearValue> clearValues{};
		VkRenderPass renderpass{};
	};
}