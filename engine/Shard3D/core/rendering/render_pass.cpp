#include "../../s3dpch.h"
#include "render_pass.h"

// Basic RenderPass abstraction (single subpass)

namespace Shard3D::Rendering {
	RenderPass::RenderPass(EngineDevice& device, const std::vector<AttachmentInfo>& attachments) : engineDevice(device) {
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		float noEditBgColor[3]{};
		noEditBgColor[0] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorR"));
		noEditBgColor[1] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorG"));
		noEditBgColor[2] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorB"));

		std::vector<VkAttachmentDescription> attachmentDescriptions;
		attachmentDescriptions.resize(attachments.size());

		for (int i = 0; i < attachmentDescriptions.size(); i++) {
			auto& attachmentDescription = attachmentDescriptions[i];
			if (attachments[i].frameBufferAttachment->getType() == FrameBufferAttachmentType::Resolve) 
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
			case (FrameBufferAttachmentType::Color): {
				VkAttachmentReference attachmentRef{ i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, };
				colorAttachments.push_back(attachmentRef);
			} break;
			case (FrameBufferAttachmentType::Depth): {
				VkAttachmentReference attachmentRef{ i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, };
				depthAttachments.push_back(attachmentRef);
			} break;
			case (FrameBufferAttachmentType::Resolve): {
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
			case (FrameBufferAttachmentType::Color):
				clearValues[i].color = { noEditBgColor[0], noEditBgColor[1], noEditBgColor[2], 1.0f };
				break;
			case (FrameBufferAttachmentType::Depth):
				clearValues[i].depthStencil = { 1.0f, 0 };
				break;
			}
			// Resolve attachments should be loaded with VK_LOAD_OP_DONT_CARE
		}
	}

	RenderPass::~RenderPass() {
		vkDestroyRenderPass(engineDevice.device(), renderpass, nullptr);
	}

	void RenderPass::beginRenderPass(FrameInfo& frameInfo, FrameBuffer* frameBuffer) {
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
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(frameBuffer->getDimensions().x);
		viewport.height = static_cast<float>(frameBuffer->getDimensions().y);
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

	void RenderPass::endRenderPass(FrameInfo& frameInfo) {
		vkCmdEndRenderPass(frameInfo.commandBuffer);
	}


	RenderPassAdvanced::RenderPassAdvanced(EngineDevice& device, const std::vector<AttachmentInfo>& attachments, std::vector<SubpassInfo> subpassInfos) : engineDevice(device) {
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		float noEditBgColor[3]{};
		noEditBgColor[0] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorR"));
		noEditBgColor[1] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorG"));
		noEditBgColor[2] = static_cast<float>(ini.GetDoubleValue("RENDERING", "DefaultBGColorB"));

		std::vector<VkAttachmentDescription> attachmentDescriptions;
		attachmentDescriptions.resize(attachments.size());

		for (uint32_t i = 0; i < attachments.size(); i++) {
			auto& attachmentDescription = attachmentDescriptions[i];
			SHARD3D_ASSERT(attachments[i].frameBufferAttachment->getType() != FrameBufferAttachmentType::Resolve && "RenderPassAdvanced does not support resolve attachments! Consider using RenderPass instead if you require to use resolve attachments!");
			SHARD3D_ASSERT(attachments[i].frameBufferAttachment->getDescription().samples == VK_SAMPLE_COUNT_1_BIT && "RenderPassAdvanced does not support MSAA! Consider using RenderPass instead if you require MSAA usage!");
			attachmentDescription.format = attachments[i].frameBufferAttachment->getDescription().frameBufferFormat;
			attachmentDescription.samples = attachments[i].frameBufferAttachment->getDescription().samples;
			attachmentDescription.loadOp = attachments[i].loadOp;
			attachmentDescription.storeOp = attachments[i].storeOp;
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescription.finalLayout = attachments[i].frameBufferAttachment->getDescription().finalLayout;
		}


		// Subpasses
		std::vector<SubpassDescription> subpassDescriptions{};
		for (uint32_t i = 0; i < subpassInfos.size(); i++) {
			SubpassDescription subpassDescriptionMy{};
			subpassDescriptionMy.subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescriptionMy.subpassDescription.colorAttachmentCount = 0;
			subpassDescriptionMy.subpassDescription.inputAttachmentCount = 0;

			bool hasDepth = false;
			bool hasColor = false;
			bool hasInput = false;

			for (auto& attachment : attachments) {
				if (attachment.frameBufferAttachment->getType() == FrameBufferAttachmentType::Depth) {
					hasDepth = true;
				}
			}

			for (uint32_t j = 0; j < subpassInfos[i].renderTargets.size(); j++) {
				uint32_t renderTarget = subpassInfos[i].renderTargets[j];
				if (attachments[renderTarget].frameBufferAttachment->getType() == FrameBufferAttachmentType::Depth) {
					subpassDescriptionMy.depthReference.attachment = renderTarget;
					subpassDescriptionMy.depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					hasDepth = true;
				}
				else {
					VkAttachmentReference colorReference{};
					colorReference.attachment = renderTarget;
					colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					subpassDescriptionMy.colorReferences.push_back(colorReference);

					hasColor = true;
				}
			}

			for (uint32_t j = 0; j < subpassInfos[i].subpassInputs.size(); j++) {
				uint32_t renderTarget = subpassInfos[i].subpassInputs[j];
				if (attachments[renderTarget].frameBufferAttachment->getType() == FrameBufferAttachmentType::Depth) {
					subpassDescriptionMy.depthReference.attachment = renderTarget;
					subpassDescriptionMy.depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					hasDepth = true;
				}
				else {
					VkAttachmentReference inputReference{};
					inputReference.attachment = renderTarget;
					inputReference.layout = attachments[renderTarget].frameBufferAttachment->getDescription().finalLayout;
					subpassDescriptionMy.inputReferences.push_back(inputReference);

					hasInput = true;
				}
			}

			if (hasDepth) {
				subpassDescriptionMy.subpassDescription.pDepthStencilAttachment = &subpassDescriptionMy.depthReference;
			}

			if (hasColor) {
				subpassDescriptionMy.subpassDescription.pColorAttachments = subpassDescriptionMy.colorReferences.data();
				subpassDescriptionMy.subpassDescription.colorAttachmentCount = subpassDescriptionMy.colorReferences.size();

			}

			if (hasInput) {
				subpassDescriptionMy.subpassDescription.pInputAttachments = subpassDescriptionMy.inputReferences.data();
				subpassDescriptionMy.subpassDescription.inputAttachmentCount = subpassDescriptionMy.inputReferences.size();
			}

			subpassDescriptions.push_back(std::move(subpassDescriptionMy));
		}

		// Dependencies
		std::vector<VkSubpassDependency> dependencies;
		if (subpassInfos.size() != 1) {
			dependencies.resize(subpassInfos.size() + 1);
			VkSubpassDependency& firstDependency = dependencies[0];
			firstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			firstDependency.dstSubpass = 0;
			firstDependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			firstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			firstDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			firstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			firstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			for (size_t i = 1; i < (dependencies.size() - 1); i++) {
				dependencies[i].srcSubpass = i - 1;
				dependencies[i].dstSubpass = i;
				dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}

			VkSubpassDependency& lastDependency = *(dependencies.end() - 1);
			lastDependency.srcSubpass = subpassInfos.size() - 1;
			lastDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			lastDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			lastDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			lastDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			lastDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			lastDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}
		else {
			dependencies.resize(2);
			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependencies[0].srcAccessMask = 0;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		std::vector<VkSubpassDescription> subpassDescriptionsVK{};
		for (uint32_t i = 0; i < subpassDescriptions.size(); i++) {
			subpassDescriptionsVK.push_back(subpassDescriptions[i].subpassDescription);
		}

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		renderPassInfo.pAttachments = attachmentDescriptions.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescriptionsVK.size());
		renderPassInfo.pSubpasses = subpassDescriptionsVK.data();
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &renderpass) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderpass");
		}

		clearValues.resize(attachments.size());
		for (uint32_t i = 0; i < attachments.size(); i++) {
			if (attachments[i].frameBufferAttachment->getType() == FrameBufferAttachmentType::Depth) 
				clearValues[i].depthStencil = { 1.0f, 0 };		
			else
				clearValues[i].color = { noEditBgColor[0], noEditBgColor[1], noEditBgColor[2], 1.0f };
			
		}
	}

	RenderPassAdvanced::~RenderPassAdvanced() {
		vkDestroyRenderPass(engineDevice.device(), renderpass, nullptr);
	}

	void RenderPassAdvanced::beginRenderPass(VkCommandBuffer commandBuffer, FrameBuffer* frameBuffer) {
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderpass;
		renderPassBeginInfo.framebuffer = frameBuffer->getFrameBuffer();
		renderPassBeginInfo.renderArea.extent.width = frameBuffer->getDimensions().x;
		renderPassBeginInfo.renderArea.extent.height = frameBuffer->getDimensions().y;
		renderPassBeginInfo.clearValueCount = clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(frameBuffer->getDimensions().x);
		viewport.height = static_cast<float>(frameBuffer->getDimensions().y);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = frameBuffer->getDimensions().x;
		scissor.extent.height = frameBuffer->getDimensions().y;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void RenderPassAdvanced::endRenderPass(VkCommandBuffer commandBuffer) {
		vkCmdEndRenderPass(commandBuffer);
	}

	void RenderPassAdvanced::nextSubpass(VkCommandBuffer commandBuffer) {
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
	}
}