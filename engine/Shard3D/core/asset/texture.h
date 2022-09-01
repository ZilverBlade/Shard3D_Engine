#pragma once
#include "../vulkan_api/device.h"
#include "../../s3dstd.h"
namespace Shard3D {
    inline namespace Resources {
        struct TextureLoadInfo {
            VkFilter filter = VK_FILTER_LINEAR;
            VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        };
        class Texture2D {
        public:
            Texture2D(EngineDevice& device, const std::string& textureFilepath, TextureLoadInfo loadInfo);
            Texture2D(EngineDevice& device);
            Texture2D(
                EngineDevice& device,
                VkFormat format,
                VkExtent3D extent,
                VkImageUsageFlags usage,
                VkSampleCountFlagBits sampleCount);
            ~Texture2D();

            // delete copy constructors
            Texture2D(const Texture2D&) = delete;
            Texture2D& operator=(const Texture2D&) = delete;

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
            static uPtr<Texture2D> createTextureFromFile(
                EngineDevice& device, const std::string& filepath, TextureLoadInfo loadInfo);
            static uPtr<Texture2D> createEmptyTexture(
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
}