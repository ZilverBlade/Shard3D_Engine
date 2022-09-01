#include "../../s3dpch.h"
#include "renderpass.h"

// Basic RenderPass abstraction (single subpass)

namespace Shard3D {
	SimpleRenderPass::SimpleRenderPass(EngineDevice& device, const std::vector<AttachmentInfo>& attachments) : engineDevice(device) {
		std::vector<VkAttachmentDescription> attachmentDescriptions;
		attachmentDescriptions.resize(attachments.size());

		for (int i = 0; i < attachmentDescriptions.size(); i++) {
			auto& attachmentDescription = attachmentDescriptions[i];
			if (attachments[i].frameBufferAttachment->getType() == AttachmentType::Resolve) 
				SHARD3D_ASSERT(attachments[i].loadOp == VK_ATTACHMENT_LOAD_OP_DONT_CARE && "Resolve attachments should have VK_ATTACHMENT_LOAD_OP_DONT_CARE set as the loadOp!");
			attachmentDescription.format = attachments[i].frameBufferAttachment->getDescription().frameBufferFormat;
			attachmentDescription.samples = attachments[i].frameBufferAttachment->getDescription().samples;
			attachmentDescription.loadOp = attachments[i].loadOp;
			attachmentDescription.storeOp = attachments[i].storeOp;
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescription.finalLayout = attachments[i].frameBufferAttachment->getDescription().finalLayout;
		}

		std::vector<VkAttachmentReference> colorAttachments{};
		std::vector<VkAttachmentReference> depthAttachments{};
		std::vector<VkAttachmentReference> resolveAttachments{};
		VkSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.pColorAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		for (uint32_t i = 0; i < attachments.size(); i++) {
			switch (attachments[i].frameBufferAttachment->getType()) {
			case (AttachmentType::Color): {
				VkAttachmentReference attachmentRef{ i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, };
				colorAttachments.push_back(attachmentRef);
			} break;
			case (AttachmentType::Depth): {
				VkAttachmentReference attachmentRef{ i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, };
				depthAttachments.push_back(attachmentRef);
			} break;
			case (AttachmentType::Resolve): {
				VkAttachmentReference attachmentRef{ i, attachments[i].frameBufferAttachment->getDescription().finalLayout };
				resolveAttachments.push_back(attachmentRef);
			} break;
			}
		}

		subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		if (colorAttachments.size() > 0) subpassDescription.pColorAttachments = colorAttachments.data();
		if (depthAttachments.size() > 0) subpassDescription.pDepthStencilAttachment = depthAttachments.data();
		if (resolveAttachments.size() > 0) subpassDescription.pResolveAttachments = resolveAttachments.data();

		// Use subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies{};

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		// Create the actual renderpass
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		renderPassInfo.pAttachments = attachmentDescriptions.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(engineDevice.device(), &renderPassInfo, nullptr, &renderpass) != VK_SUCCESS) {
			SHARD3D_ERROR("Failed to create render pass");
		}

		clearValues.resize(attachments.size());
		for (int i = 0; i < attachments.size(); i++) {
			switch (attachments[i].frameBufferAttachment->getType()) {
			case (AttachmentType::Color):
				clearValues[i].color = { 0.01f, 0.01f, 0.01f, 1.0f };
				break;
			case (AttachmentType::Depth):
				clearValues[i].depthStencil = { 1.0f, 0 };
				break;
			}
			// Resolve attachments should be loaded with VK_LOAD_OP_DONT_CARE
		}
	}

	SimpleRenderPass::~SimpleRenderPass() {
		vkDestroyRenderPass(engineDevice.device(), renderpass, nullptr);
	}

	void SimpleRenderPass::beginRenderPass(FrameInfo& frameInfo, FrameBuffer* frameBuffer) {
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderpass;
		renderPassBeginInfo.framebuffer = frameBuffer->getFrameBuffer();
		renderPassBeginInfo.renderArea.extent.width = frameBuffer->getDimensions().x;
		renderPassBeginInfo.renderArea.extent.height = frameBuffer->getDimensions().y;
		renderPassBeginInfo.clearValueCount = clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();
	
		vkCmdBeginRenderPass(frameInfo.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(frameBuffer->getDimensions().y);
		viewport.width = static_cast<float>(frameBuffer->getDimensions().x);
		viewport.height = -static_cast<float>(frameBuffer->getDimensions().y);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(frameInfo.commandBuffer, 0, 1, &viewport);
	
		VkRect2D scissor = {};
		scissor.extent.width = frameBuffer->getDimensions().x;
		scissor.extent.height = frameBuffer->getDimensions().y;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(frameInfo.commandBuffer, 0, 1, &scissor);
	}

	void SimpleRenderPass::endRenderPass(FrameInfo& frameInfo) {
		vkCmdEndRenderPass(frameInfo.commandBuffer);
	}
}