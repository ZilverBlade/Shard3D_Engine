#pragma once

#include "device.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <memory>
#include <string>

namespace Shard3D {
    class EngineTexture {
    public:
        EngineTexture(EngineDevice& device, const std::string& textureFilepath);
        EngineTexture(
            EngineDevice& device,
            VkFormat format,
            VkExtent3D extent,
            VkImageUsageFlags usage,
            VkSampleCountFlagBits sampleCount);
        ~EngineTexture();

        // delete copy constructors
        EngineTexture(const EngineTexture&) = delete;
        EngineTexture& operator=(const EngineTexture&) = delete;

        VkImageView imageView() const { return mTextureImageView; }
        VkSampler sampler() const { return mTextureSampler; }
        VkImage getImage() const { return mTextureImage; }
        VkImageView getImageView() const { return mTextureImageView; }
        VkDescriptorImageInfo getImageInfo() const { return mDescriptor; }
        VkImageLayout getImageLayout() const { return mTextureLayout; }
        VkExtent3D getExtent() const { return mExtent; }
        VkFormat getFormat() const { return mFormat; }

        void updateDescriptor();
        void transitionLayout(
            VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

        static std::unique_ptr<EngineTexture> createTextureFromFile(
            EngineDevice& device, const std::string& filepath);

    private:
        void createTextureImage(const std::string& filepath);

        void createTextureImageView(VkImageViewType viewType);
        void createTextureSampler();

        VkDescriptorImageInfo mDescriptor{};

        EngineDevice& mDevice;
        VkImage mTextureImage = nullptr;
        VkDeviceMemory mTextureImageMemory = nullptr;
        VkImageView mTextureImageView = nullptr;
        VkSampler mTextureSampler = nullptr;
        VkFormat mFormat;
        VkImageLayout mTextureLayout;
        uint32_t mMipLevels{ 1 };
        uint32_t mLayerCount{ 1 };
        VkExtent3D mExtent{};
    };

}