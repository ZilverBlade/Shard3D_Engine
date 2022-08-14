#pragma once
#include "../vulkan_api/device.h"

namespace Shard3D {
    class EngineCubemap {
    public:
        EngineCubemap(EngineDevice& device, const std::string& textureFilepath, VkFilter filter);
       
        ~EngineCubemap();

        // delete copy constructors
        EngineCubemap(const EngineCubemap&) = delete;
        EngineCubemap& operator=(const EngineCubemap&) = delete;

        VkSampler getSampler() const { return mTextureSampler; }
        VkImage getImage() const { return mTextureImage; }
        VkImageView getImageView() const { return mTextureImageView; }
        VkDescriptorImageInfo getImageInfo() const { return mDescriptor; }
        VkImageLayout getImageLayout() const { return mTextureLayout; }
        VkExtent3D getExtent() const { return mExtent; }
        VkFormat getFormat() const { return mFormat; }

        void updateDescriptor();

        static uPtr<EngineCubemap> createCubemapFromFolder(
            EngineDevice& device, const std::string& folderpath, VkFilter filter);
    private:
        void transitionLayout(
            VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

        void createCubemapImage(const std::string& filepath);

        void createCubemapImageView(VkImageViewType viewType);
        void createCubemapSampler();

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
        uint32_t mLayerCount{ 6 };
        VkExtent3D mExtent{};
    };

}