#include "../../s3dpch.h" 
#include "texture.h"
#include "cubemap.h"

// std
#include <cmath>
#include "../misc/graphics_settings.h"
#include <stb_image.h>

namespace Shard3D {
    EngineCubemap::EngineCubemap(EngineDevice& device, const std::string& textureFilepath, VkFilter filter) : mDevice{ device } {
        mFilter = filter;
        createCubemapImage(textureFilepath);
        createCubemapImageView(VK_IMAGE_VIEW_TYPE_2D);
        createCubemapSampler();
        updateDescriptor();
    }
    EngineCubemap::~EngineCubemap() {
        vkDestroySampler(mDevice.device(), mTextureSampler, nullptr);
        vkDestroyImageView(mDevice.device(), mTextureImageView, nullptr);
        vkDestroyImage(mDevice.device(), mTextureImage, nullptr);
        vkFreeMemory(mDevice.device(), mTextureImageMemory, nullptr);
    }

    uPtr<EngineCubemap> EngineCubemap::createCubemapFromFolder(
        EngineDevice& device, const std::string& folderpath, VkFilter filter) {
        return make_uPtr<EngineCubemap>(device, folderpath, filter);
    }

 
    void EngineCubemap::updateDescriptor() {
        mDescriptor.sampler = mTextureSampler;
        mDescriptor.imageView = mTextureImageView;
        mDescriptor.imageLayout = mTextureLayout;
    }
    void EngineCubemap::createCubemapImage(const std::string& filepath) {
        int texWidth, texHeight, texChannels;

        std::vector<stbi_uc*> pixels;
        pixels.push_back(EngineTexture::getSTBImage(filepath + "/top.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha));
        pixels.push_back(EngineTexture::getSTBImage(filepath + "/left.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha));
        pixels.push_back(EngineTexture::getSTBImage(filepath + "/right.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha));
        pixels.push_back(EngineTexture::getSTBImage(filepath + "/front.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha));
        pixels.push_back(EngineTexture::getSTBImage(filepath + "/bottom.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha));
        pixels.push_back(EngineTexture::getSTBImage(filepath + "/back.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha));
        VkDeviceSize imageSize = 512 * 512 * 4 * 6;
        
        {int i = 0;
            for (auto& px : pixels) {
                if (!px) {
                    SHARD3D_FATAL(std::string("failed to load texture image! Tried to load cubemap index " + std::to_string(i)));
                }
                i += 1;
            }
        }

        mMipLevels = 1;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        mDevice.createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        VkDeviceSize layerSize = imageSize / 6;

        stbi_uc* data = nullptr;
        vkMapMemory(mDevice.device(), stagingBufferMemory, 0, imageSize, 0, (void**)&data);

        for (int i = 0; i < pixels.size(); i++) {
            memcpy_s(data + layerSize * i, static_cast<size_t>(sizeof(data) + layerSize * i), pixels.at(i), static_cast<size_t>(imageSize));
        }

        vkUnmapMemory(mDevice.device(), stagingBufferMemory);

        for (uint32_t i = 0; i < 6; i++) {
            EngineTexture::freeSTBImage(pixels[i]);
        }

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
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

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

    void EngineCubemap::createCubemapImageView(VkImageViewType viewType) {
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

    void EngineCubemap::createCubemapSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = mFilter;
        samplerInfo.minFilter = mFilter;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = static_cast<float>(GraphicsSettings::get().maxAnisotropy);
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        // this fields useful for percentage close filtering for shadow maps
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mMipLevels);

        if (vkCreateSampler(mDevice.device(), &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS) {
            SHARD3D_FATAL("failed to create texture sampler!");
        }
    }

    void EngineCubemap::transitionLayout(
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