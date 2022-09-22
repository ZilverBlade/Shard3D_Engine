#pragma once

#include "../vulkan_api/device.h"
#include <glm/glm.hpp>

namespace Shard3D {
	inline namespace Rendering {
		struct FrameBufferAttachmentDescription {
			VkFormat frameBufferFormat;
			VkImageAspectFlags imageAspect;
			glm::ivec3 dimensions;
			VkImageUsageFlags usage;
			VkImageLayout finalLayout;
			VkSampleCountFlagBits samples;
		};

		enum class FrameBufferAttachmentType {
			Color,
			Depth,
			Resolve,
			Storage
		};

		class FrameBufferAttachment {
		public:
			FrameBufferAttachment(EngineDevice& device, FrameBufferAttachmentDescription&& attachmentDescription, FrameBufferAttachmentType attachmentType, bool hasSampler = true);
			~FrameBufferAttachment();

			DELETE_COPY(FrameBufferAttachment)
			DELETE_MOVE(FrameBufferAttachment)

			inline VkImage getImage() { return image; }
			inline VkImageView getImageView() { return imageView; }
			inline VkSampler getSampler() { SHARD3D_ASSERT(sampler && "Sampler was never created or is of type for a subpass!"); return sampler; }
			inline VkImageLayout getImageLayout() { return finalLayout; }
			inline VkDeviceMemory getDeviceMemory() { return imageMemory; }
			inline glm::ivec3 getDimensions() { return dimensions; }
			inline FrameBufferAttachmentDescription& getDescription() { return description; }
			inline FrameBufferAttachmentType getType() { return _attachmentType; }
			inline VkDescriptorImageInfo getDescriptor() { return descriptor; }

			void resize(glm::ivec3 newDimensions);
		private:
			void destroy();
			void create(EngineDevice& device, const FrameBufferAttachmentDescription& attachmentDescription, FrameBufferAttachmentType attachmentType, bool hasSampler = true);

			VkImage image{};
			VkImageView imageView{};
			VkSampler sampler{};
			VkImageLayout finalLayout{};
			VkDeviceMemory imageMemory{};
			VkDescriptorImageInfo descriptor{};

			EngineDevice& engineDevice;
			FrameBufferAttachmentDescription description;
			FrameBufferAttachmentType _attachmentType;

			glm::ivec3 dimensions{};
		};

		class FrameBuffer {
		public:
			FrameBuffer(EngineDevice& device, VkRenderPass renderPass, const std::vector<Rendering::FrameBufferAttachment*>& attachments);
			~FrameBuffer();

			DELETE_COPY(FrameBuffer);
			DELETE_MOVE(FrameBuffer);

			inline glm::ivec3 getDimensions() { return { width, height, depth }; }
			inline VkFramebuffer getFrameBuffer() { return frameBuffer; }
			void resize(glm::ivec3 newDimensions, VkRenderPass renderpass);
		private:
			void create(EngineDevice& device, VkRenderPass renderPass, const std::vector<FrameBufferAttachment*>& _attachments);
			void destroy();

			uint32_t width{};
			uint32_t height{};
			uint32_t depth{};

			VkFramebuffer frameBuffer{};

			std::vector<FrameBufferAttachment*> attachments;
			EngineDevice& engineDevice;
		};
	}
}