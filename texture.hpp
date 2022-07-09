#pragma once
#include "s3dtpch.h"
#include "device.hpp"

namespace Shard3D {
    class EngineTexture {
    public:
        EngineTexture(EngineDevice& device, const std::string& textureFilepath, VkFilter filter);
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

        VkSampler getSampler() const { return mTextureSampler; }
        VkImage getImage() const { return mTextureImage; }
        VkImageView getImageView() const { return mTextureImageView; }
        VkDescriptorImageInfo getImageInfo() const { return mDescriptor; }
        VkImageLayout getImageLayout() const { return mTextureLayout; }
        VkExtent3D getExtent() const { return mExtent; }
        VkFormat getFormat() const { return mFormat; }

        void updateDescriptor();
        
        static unsigned char* getSTBImage(const std::string& filepath, int* x, int* y, int* comp, int req_comp);
        static void freeSTBImage(void* pixels);
        static std::unique_ptr<EngineTexture> createTextureFromFile(
            EngineDevice& device, const std::string& filepath, VkFilter filter);

    private:
        void transitionLayout(
            VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

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
        VkFilter mFilter;
        VkImageLayout mTextureLayout;
        uint32_t mMipLevels{ 1 };
        uint32_t mLayerCount{ 1 };
        VkExtent3D mExtent{};
    };

}