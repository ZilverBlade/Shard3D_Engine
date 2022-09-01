#include "../../s3dpch.h" 
#include "texture.h"

// libs
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// std
#include <cmath>
#include "../misc/graphics_settings.h"

namespace Shard3D::Resources {
    Texture2D::Texture2D(EngineDevice& device, const std::string& textureFilepath, TextureLoadInfo loadInfo) : mDevice{ device } {
        mLoadInfo = loadInfo;
        createTextureImage(textureFilepath);
        createTextureImageView(VK_IMAGE_VIEW_TYPE_2D);
        createTextureSampler();
        updateDescriptor();
    }
    Texture2D::Texture2D(EngineDevice& device) : mDevice{ device } { 
        mLoadInfo.filter = VK_FILTER_LINEAR;
        mLoadInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        mLoadInfo.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        createBlankTextureImage();
        updateDescriptor();
    }
  
    Texture2D::~Texture2D() {
        vkDestroySampler(mDevice.device(), mTextureSampler, nullptr);
        vkDestroyImageView(mDevice.device(), mTextureImageView, nullptr);
        vkDestroyImage(mDevice.device(), mTextureImage, nullptr);
        vkFreeMemory(mDevice.device(), mTextureImageMemory, nullptr);
    }

    uPtr<Texture2D> Texture2D::createTextureFromFile(
        EngineDevice& device, const std::string& filepath, TextureLoadInfo loadInfo) {
        return make_uPtr<Texture2D>(device, filepath, loadInfo);
    }

    uPtr<Texture2D> Texture2D::createEmptyTexture(EngineDevice& device)
    {
        return make_uPtr<Texture2D>(device);
    }

    void Texture2D::updateDescriptor() {
        mDescriptor.sampler = mTextureSampler;
        mDescriptor.imageView = mTextureImageView;
        mDescriptor.imageLayout = mTextureLayout;
    }
    unsigned char* Texture2D::getSTBImage(const std::string& filepath, int* x, int* y, int* comp, int req_comp) {
        SHARD3D_LOG("Loading STBImage {0}", filepath);
        return stbi_load(filepath.c_str(), x, y, comp, req_comp);
    }
    void Texture2D::freeSTBImage(void* pixels) {
        stbi_image_free(pixels);
    }
    void Texture2D::createBlankTextureImage() {

        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageCreateInfo.extent = { 1920, 1080, 1 };
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        // Image will be sampled in the fragment shader and used as storage target in the compute shader
        imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
        imageCreateInfo.flags = 0;
        // If compute and graphics queue family indices differ, we create an image that can be shared between them
        // This can result in worse performance than exclusive sharing mode, but save some synchronization to keep the sample simple
        mFormat = VK_FORMAT_R8G8B8A8_SRGB;
        mExtent = imageCreateInfo.extent;

        mDevice.createImageWithInfo(
            imageCreateInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mTextureImage,
            mTextureImageMemory);
  
        // If we generate mip maps then the final image will alerady be READ_ONLY_OPTIMAL
        // mDevice.generateMipmaps(mTextureImage, mFormat, texWidth, texHeight, mMipLevels);
        mTextureLayout = VK_IMAGE_LAYOUT_GENERAL;


        // Create sampler
        VkSamplerCreateInfo sampler{};
        sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler.magFilter = VK_FILTER_LINEAR;
        sampler.minFilter = VK_FILTER_LINEAR;
        sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        sampler.addressModeV = sampler.addressModeU;
        sampler.addressModeW = sampler.addressModeU;
        sampler.mipLodBias = 0.0f;
        sampler.maxAnisotropy = 1.0f;
        sampler.compareOp = VK_COMPARE_OP_NEVER;
        sampler.minLod = 0.0f;
        sampler.maxLod = mMipLevels;
        sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        vkCreateSampler(mDevice.device(), &sampler, nullptr, &mTextureSampler);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = VK_NULL_HANDLE;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8_SRGB;
        viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        viewInfo.image = mTextureImage;
         vkCreateImageView(mDevice.device(), &viewInfo, nullptr, &mTextureImageView);
    }
    void Texture2D::createTextureImage(const std::string& filepath) {
        int texWidth, texHeight, texChannels;
        // stbi_set_flip_vertically_on_load(1);  // todo determine why texture coordinates are flipped
        stbi_uc* pixels =
            stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            SHARD3D_FATAL(std::string("failed to load texture image! Tried to load '" + filepath + "'"));
        }

        // mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        mMipLevels = 1;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        mDevice.createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(mDevice.device(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(mDevice.device(), stagingBufferMemory);

        stbi_image_free(pixels);

        mFormat = VK_FORMAT_R8G8B8A8_SRGB;
        mExtent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = mExtent;
        imageInfo.mipLevels = mMipLevels;
        imageInfo.arrayLayers = mLayerCount;
        imageInfo.format = mFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        mDevice.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mTextureImage,
            mTextureImageMemory);
        mDevice.transitionImageLayout(
            mTextureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mMipLevels,
            mLayerCount);
        mDevice.copyBufferToImage(
            stagingBuffer,
            mTextureImage,
            static_cast<uint32_t>(texWidth),
            static_cast<uint32_t>(texHeight),
            mLayerCount);

        // comment this out if using mips
        mDevice.transitionImageLayout(
            mTextureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            mMipLevels,
            mLayerCount);

        // If we generate mip maps then the final image will alerady be READ_ONLY_OPTIMAL
        // mDevice.generateMipmaps(mTextureImage, mFormat, texWidth, texHeight, mMipLevels);
        mTextureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vkDestroyBuffer(mDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(mDevice.device(), stagingBufferMemory, nullptr);
    }

    void Texture2D::createTextureImageView(VkImageViewType viewType) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mTextureImage;
        viewInfo.viewType = viewType;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mMipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = mLayerCount;

        if (vkCreateImageView(mDevice.device(), &viewInfo, nullptr, &mTextureImageView) != VK_SUCCESS) {
            SHARD3D_FATAL("failed to create texture image view!");
        }
    }

    void Texture2D::createTextureSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = mLoadInfo.filter;
        samplerInfo.minFilter = mLoadInfo.filter;

        samplerInfo.addressModeU = mLoadInfo.addressMode;
        samplerInfo.addressModeV = mLoadInfo.addressMode;
        samplerInfo.addressModeW = mLoadInfo.addressMode;

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = static_cast<float>(GraphicsSettings::get().maxAnisotropy);
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        // this fields useful for percentage close filtering for shadow maps
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = mLoadInfo.mipmapMode;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mMipLevels);

        if (vkCreateSampler(mDevice.device(), &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS) {
            SHARD3D_FATAL("failed to create texture sampler!");
        }
    }

    void Texture2D::transitionLayout(
        VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = mTextureImage;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mMipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = mLayerCount;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (mFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || mFormat == VK_FORMAT_D24_UNORM_S8_UINT) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            // This says that any cmd that acts in color output or after (dstStage)
            // that needs read or write access to a resource
            // must wait until all previous read accesses in fragment shader
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            barrier.dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }
        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage,
            destinationStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);
    }
}