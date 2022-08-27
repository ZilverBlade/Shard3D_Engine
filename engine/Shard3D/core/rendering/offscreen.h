#pragma once
//
// Created by lukas on 27/11/2021
//

#include "../vulkan_api/device.h"

#include "../misc/frame_info.h"

namespace Shard3D {
    class OffScreen {
        struct FrameBufferAttachment {
            VkImage image;
            VkDeviceMemory mem;
            VkImageView view;
        };
        struct OffscreenPass {
            uint32_t width = 1280;
            uint32_t height = 720;
            VkFramebuffer frameBuffer;
            FrameBufferAttachment color, depth, colorResolve;
            VkRenderPass renderPass;
            VkSampler sampler;
            VkDescriptorImageInfo descriptor;
        };

        std::array<VkClearValue, 2> clearValues{};
        VkDeviceSize offsets[1] = { 0 };
    public:
        OffScreen(EngineDevice &device);
        //void Init(Device device);
        ~OffScreen() {
            vkDestroyImageView(m_Device.device(), pass.color.view, nullptr);
            vkDestroyImage(m_Device.device(), pass.color.image, nullptr);
            vkFreeMemory(m_Device.device(), pass.color.mem, nullptr);

            // Depth attachment
            vkDestroyImageView(m_Device.device(), pass.depth.view, nullptr);
            vkDestroyImage(m_Device.device(), pass.depth.image, nullptr);
            vkFreeMemory(m_Device.device(), pass.depth.mem, nullptr);
            
            // Depth attachment
            vkDestroyImageView(m_Device.device(), pass.colorResolve.view, nullptr);
            vkDestroyImage(m_Device.device(), pass.colorResolve.image, nullptr);
            vkFreeMemory(m_Device.device(), pass.colorResolve.mem, nullptr);

            vkDestroyRenderPass(m_Device.device(), pass.renderPass, nullptr);
            vkDestroySampler(m_Device.device(), pass.sampler, nullptr);
            vkDestroyFramebuffer(m_Device.device(), pass.frameBuffer, nullptr);
        }

        VkSampler getSampler() { return pass.sampler; }
        VkImageView getImageView() { return pass.colorResolve.view; }
        VkRenderPass getRenderPass() { return pass.renderPass; }
        const OffscreenPass getPass() { return pass; }
        void setViewportSize(const glm::vec2& size) {
            pass.width = size.x;
            pass.height = size.y;

            createImages();
        }

        void start(FrameInfo& frameInfo);
        void end(FrameInfo& frameInfo);
    private:
        void createImages();

        OffscreenPass pass;
        EngineDevice &m_Device;
    };
}
