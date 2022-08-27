#pragma once
#include "../vulkan_api/device.h"
#include "../../s3dstd.h"
namespace Shard3D {
    struct TextureLoadInfo {
        VkFilter filter = VK_FILTER_LINEAR;
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    };
    class EngineTexture {
    public:
        EngineTexture(EngineDevice& device, const std::string& textureFilepath, TextureLoadInfo loadInfo);
        EngineTexture(EngineDevice& device);
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
        void createBlankTextureImage();
        static uPtr<EngineTexture> createTextureFromFile(
            EngineDevice& device, const std::string& filepath, TextureLoadInfo loadInfo);
        static uPtr<EngineTexture> createEmptyTexture(
            EngineDevice& device);

    private:
        void transitionLayout(
            VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

        void createTextureImage(const std::string& filepath);

        void createTextureImageView(VkImageViewType viewType);
        void createBlankTextureImageView(VkImageViewType viewType);
        void createTextureSampler();

        VkDescriptorImageInfo mDescriptor{};

        EngineDevice& mDevice;
        VkImage mTextureImage = nullptr;
        VkDeviceMemory mTextureImageMemory = nullptr;
        VkImageView mTextureImageView = nullptr;
        VkSampler mTextureSampler = nullptr;
        VkFormat mFormat;
        TextureLoadInfo mLoadInfo;
        VkImageLayout mTextureLayout;
        uint32_t mMipLevels{ 1 };
        uint32_t mLayerCount{ 1 };
        VkExtent3D mExtent{};
    };

}