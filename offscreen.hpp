#pragma once
//
// Created by lukas on 27.11.21.
//

#include <vulkan/vulkan.h>
#include "device.hpp"

#include <memory>
#include "frame_info.hpp"

namespace Shard3D {

    struct FrameBufferAttachment {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    };
    struct OffscreenPass {
        uint32_t width, height;
        VkFramebuffer frameBuffer;
        FrameBufferAttachment color, depth;
        VkRenderPass renderPass;
        VkSampler sampler;
        VkDescriptorImageInfo descriptor;
    };

    class OffScreen {
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

            vkDestroyRenderPass(m_Device.device(), pass.renderPass, nullptr);
            vkDestroySampler(m_Device.device(), pass.sampler, nullptr);
            vkDestroyFramebuffer(m_Device.device(), pass.frameBuffer, nullptr);
        }

        VkSampler getSampler() { return pass.sampler; }
        VkImageView getImageView() { return pass.color.view; }
        VkRenderPass getRenderPass() { return pass.renderPass; }
        void setViewportSize(const glm::vec2& size) {
            pass.width = size.x;
            pass.height = size.y;

            createImages();
        }

        void start(FrameInfo frameInfo);
        void end(FrameInfo frameInfo);
    private:
        void createImages();

        OffscreenPass pass;
        EngineDevice &m_Device;
        bool after = false;
    };
}
