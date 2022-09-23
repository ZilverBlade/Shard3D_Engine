#include "sync.h"

namespace Shard3D {
	Synchronization::Synchronization(SynchronizationType syncType_, VkPipelineStageFlags specialStage) : syncType(syncType_) {
		switch (syncType_) {
		case(SynchronizationType::GraphicsToCompute):
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			srcStageMask = specialStage;
			break;
		case(SynchronizationType::ComputeToCompute):
			srcStageMask = dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case(SynchronizationType::ComputeToGraphics):
			dstStageMask = specialStage;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		}
	}
	void Synchronization::addImageBarrier(
		SynchronizationAttachment syncAttachment_, VkImage toSyncImage, VkImageSubresourceRange subresourceRange, VkImageLayout newLayout) {
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.newLayout = newLayout;

		/* .image and .subresourceRange should identify image subresource accessed */
		imageMemoryBarrier.image = toSyncImage;
		imageMemoryBarrier.subresourceRange = subresourceRange;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		// check if the new layout will be written to
		if (newLayout == VK_IMAGE_LAYOUT_GENERAL)
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		else
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		// Graphics old layout is always undefined
		if (syncType == SynchronizationType::GraphicsToCompute) {
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			switch (syncAttachment_) {
			case (SynchronizationAttachment::Color):
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				SHARD3D_ASSERT(srcStageMask == VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT && "Invalid synchronization!");
				break;
			case (SynchronizationAttachment::Depth):
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				SHARD3D_ASSERT(srcStageMask == (VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT) && "Invalid synchronization!");
				break;
			}
		}
		else { // If origin is from compute the image must have been written to
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;	
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		}
		imageMemoryBarriers.push_back(imageMemoryBarrier);
	}

	void Synchronization::syncBarrier(VkCommandBuffer commandBuffer) {
		vkCmdPipelineBarrier(
			commandBuffer,
			srcStageMask,				// srcStageMask
			dstStageMask,				// dstStageMask
			0,
			memoryBarriers.size(),		// memoryBarrierCount         
			memoryBarriers.data(),		// pMemoryBarriers
			0,							// bufferMemoryBarrierCount
			nullptr,					// pBufferMemoryBarriers
			imageMemoryBarriers.size(),	// imageMemoryBarrierCount
			imageMemoryBarriers.data()	// pImageMemoryBarriers
		);
	}


}