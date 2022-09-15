#include "../../s3dpch.h"
#include "frame_buffer.h"
#include <Shlwapi.h>
namespace Shard3D {
	FrameBufferAttachment::FrameBufferAttachment(EngineDevice& device, FrameBufferAttachmentDescription&& attachmentDescription, FrameBufferAttachmentType attachmentType, bool hasSampler) : engineDevice(device), description(attachmentDescription), _attachmentType(attachmentType) {
		create(engineDevice, attachmentDescription, this->_attachmentType, hasSampler);
	}
	void FrameBufferAttachment::destroy() {
		vkDestroyImageView(engineDevice.device(), imageView, nullptr);
		vkDestroyImage(engineDevice.device(), image, nullptr);
		vkDestroySampler(engineDevice.device(), sampler, nullptr);
		vkFreeMemory(engineDevice.device(), imageMemory, nullptr);
	}
	FrameBufferAttachment::~FrameBufferAttachment() {
		destroy();
	}

	void FrameBufferAttachment::resize(glm::ivec3 newDimensions) {
		destroy();
		description.dimensions = newDimensions;
		create(engineDevice, description, this->_attachmentType, this->sampler);
	}	

	void FrameBufferAttachment::create(EngineDevice& device, const FrameBufferAttachmentDescription& attachmentDescription, FrameBufferAttachmentType attachmentType, bool hasSampler) {
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = description.frameBufferFormat;
		imageCreateInfo.extent.width = attachmentDescription.dimensions.x;
		imageCreateInfo.extent.height = attachmentDescription.dimensions.y;
		imageCreateInfo.extent.depth = attachmentDescription.dimensions.z;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = attachmentDescription.samples;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the color attachment
		imageCreateInfo.usage = attachmentDescription.usage | (VK_IMAGE_USAGE_SAMPLED_BIT * hasSampler);
		VkMemoryAllocateInfo memAlloc = {};
		memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements memReqs;

		if (vkCreateImage(device.device(), &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
			SHARD3D_ERROR("Failed to create image!");
		}
		vkGetImageMemoryRequirements(device.device(), image, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = device.findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (vkAllocateMemory(device.device(), &memAlloc, nullptr, &imageMemory) != VK_SUCCESS) {
			SHARD3D_ERROR("Failed to allocate memory!");
		}
		if (vkBindImageMemory(device.device(), image, imageMemory, 0) != VK_SUCCESS) {
			SHARD3D_ERROR("Failed to bind memory!");
		}

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = description.frameBufferFormat;
		imageViewCreateInfo.subresourceRange = {};
		imageViewCreateInfo.subresourceRange.aspectMask = description.imageAspect;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.image = image;

		if (vkCreateImageView(device.device(), &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
			SHARD3D_ERROR("Failed to create image view!");
		}

		if (hasSampler) {
			// Create sampler to sample from the attachment in the fragment shader
			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfo.addressModeV = samplerInfo.addressModeU;
			samplerInfo.addressModeW = samplerInfo.addressModeU;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.maxAnisotropy = 1.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 1.0f;
			samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
				SHARD3D_ERROR("Failed to create sampler");
			}

			// Fill a descriptor for later use in a descriptor set
			descriptor.imageLayout = attachmentDescription.finalLayout;
			descriptor.imageView = imageView;
			descriptor.sampler = sampler;
		}
		dimensions = { attachmentDescription.dimensions.x, attachmentDescription.dimensions.y, attachmentDescription.dimensions.z };
	}

	
	void FrameBuffer::destroy()
	{
		vkDestroyFramebuffer(engineDevice.device(), frameBuffer, nullptr);
	}
	void FrameBuffer::create(EngineDevice& device, VkRenderPass renderPass, const std::vector<FrameBufferAttachment*>& _attachments) {
		std::vector<VkImageView> imageViews;
		for (auto& attachment : _attachments) {
			imageViews.push_back(attachment->getImageView());
		}

		width = static_cast<uint32_t>(_attachments[0]->getDimensions().x);
		height = static_cast<uint32_t>(_attachments[0]->getDimensions().y);
		depth = static_cast<uint32_t>(_attachments[0]->getDimensions().z);

		VkFramebufferCreateInfo fbufferCreateInfo{};
		fbufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufferCreateInfo.pNext = nullptr;
		fbufferCreateInfo.flags = 0;
		fbufferCreateInfo.renderPass = renderPass;
		fbufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
		fbufferCreateInfo.pAttachments = imageViews.data();
		fbufferCreateInfo.width = width;
		fbufferCreateInfo.height = height;
		fbufferCreateInfo.layers = depth;

		if (vkCreateFramebuffer(device.device(), &fbufferCreateInfo, nullptr, &frameBuffer) != VK_SUCCESS) {
			SHARD3D_ERROR("Failed to create framebuffer!");
		}
	}

	FrameBuffer::FrameBuffer(EngineDevice& device, VkRenderPass renderPass, const std::vector<FrameBufferAttachment*>& _attachments) : engineDevice(device) {
		create(device, renderPass, _attachments);
		this->attachments.reserve(_attachments.size());
		for (FrameBufferAttachment* attachment : _attachments) {
			this->attachments.push_back(attachment);
		}
	}

	FrameBuffer::~FrameBuffer() {
		destroy();
		
	}

	void FrameBuffer::resize(glm::ivec3 newDimensions, VkRenderPass renderpass) {
		for (FrameBufferAttachment* attachment : attachments) {
			attachment->resize(newDimensions);
		}
		destroy();
		create(this->engineDevice, renderpass, attachments);
	}

}