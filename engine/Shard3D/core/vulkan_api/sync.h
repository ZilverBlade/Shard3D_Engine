#pragma once
#include "../../core.h"
namespace Shard3D {

	enum class SynchronizationAttachment {
		None, // Only use for ComputeToXXX syncs
		Depth,
		Color
	};

	enum class SynchronizationType {
		ComputeToCompute,
		ComputeToGraphics,
		GraphicsToCompute
	};

//  sync'y
	class Synchronization {
	public:
		DELETE_COPY(Synchronization)
		Synchronization(SynchronizationType syncType_, VkPipelineStageFlags specialStage = 0);
		void addImageBarrier(SynchronizationAttachment syncAttachment_, VkImage toSyncImage, VkImageSubresourceRange subresourceRange, VkImageLayout newLayout);
		void syncBarrier(VkCommandBuffer commandBuffer);
		void clearBarriers() { memoryBarriers.clear(); imageMemoryBarriers.clear(); }
	protected:
		VkPipelineStageFlags srcStageMask{};
		VkPipelineStageFlags dstStageMask{};
		std::vector<VkMemoryBarrier> memoryBarriers;
		std::vector<VkImageMemoryBarrier> imageMemoryBarriers;
		SynchronizationType syncType;
	};
//  this is the sync'y
}