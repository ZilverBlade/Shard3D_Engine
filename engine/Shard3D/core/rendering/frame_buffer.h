#pragma once

#include "../vulkan_api/device.h"
#include <glm/glm.hpp>

namespace Shard3D {

	struct FrameBufferAttachmentDescription {
		VkFormat frameBufferFormat;
		VkImageAspectFlags imageAspect;
		glm::ivec3 dimensions;
		VkImageUsageFlagBits usage;
		VkImageLayout finalLayout;
		VkSampleCountFlagBits samples;
	};

	enum class AttachmentType {
		Color,
		Depth,
		Resolve
	};

	class FrameBufferAttachment {
	public:
		FrameBufferAttachment(EngineDevice& device, FrameBufferAttachmentDescription&& attachmentDescription, AttachmentType attachmentType);
		~FrameBufferAttachment();

		FrameBufferAttachment(const FrameBufferAttachment&) = delete;
		FrameBufferAttachment& operator=(const FrameBufferAttachment&) = delete;
		FrameBufferAttachment(FrameBufferAttachment&&) = delete;
		FrameBufferAttachment& operator=(FrameBufferAttachment&&) = delete;

		inline VkImage getImage() { return image; }
		inline VkImageView getImageView() { return imageView; }
		inline VkSampler getSampler() { return sampler; }
		inline VkImageLayout getImageLayout() { return finalLayout; }
		inline VkDeviceMemory getDeviceMemory() { return imageMemory; }
		inline glm::ivec3 getDimensions() { return dimensions; }
		inline FrameBufferAttachmentDescription& getDescription() { return description; }
		inline AttachmentType getType() { return _attachmentType; }
		inline VkDescriptorImageInfo getDescriptor() { return descriptor; }
		
	private:
		VkImage image{};
		VkImageView imageView{};
		VkSampler sampler{};
		VkImageLayout finalLayout{};
		VkDeviceMemory imageMemory{};
		VkDescriptorImageInfo descriptor{};

		EngineDevice& engineDevice;
		FrameBufferAttachmentDescription description;
		AttachmentType _attachmentType;

		glm::ivec3 dimensions{};
	};

	class FrameBuffer {
	public:
		FrameBuffer(EngineDevice& device, VkRenderPass renderPass, const std::vector<FrameBufferAttachment*>& attachments);
		~FrameBuffer();

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;

		inline glm::ivec3 getDimensions() { return {width, height, depth}; }
		inline VkFramebuffer getFrameBuffer() { return frameBuffer; }

	private:
		uint32_t width{};
		uint32_t height{};
		uint32_t depth{};

		VkFramebuffer frameBuffer{};


		EngineDevice& engineDevice;
	};

}