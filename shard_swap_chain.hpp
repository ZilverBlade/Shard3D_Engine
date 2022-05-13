#pragma once
#include "shard_device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

#include <fstream>


namespace shard {

    class ShardSwapChain {
         public:
              static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    
              ShardSwapChain(ShardDevice &deviceRef, VkExtent2D windowExtent);
              ShardSwapChain(ShardDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<ShardSwapChain> previous);
              ~ShardSwapChain();
    
              ShardSwapChain(const ShardSwapChain &) = delete;
              ShardSwapChain& operator=(const ShardSwapChain &) = delete;
              ShardSwapChain() = default;
    
              VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
              VkRenderPass getRenderPass() { return renderPass; }
              VkImageView getImageView(int index) { return swapChainImageViews[index]; }
              size_t imageCount() { return swapChainImages.size(); }
              VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
              VkExtent2D getSwapChainExtent() { return swapChainExtent; }
              uint32_t width() { return swapChainExtent.width; }
              uint32_t height() { return swapChainExtent.height; }
    
        float extentAspectRatio() {
              return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }
 VkFormat findDepthFormat();

 VkResult acquireNextImage(uint32_t *imageIndex);
 VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

 bool compareSwapFormats(const ShardSwapChain& swapChain) const {
     return swapChain.swapChainDepthFormat == swapChainDepthFormat  &&
            swapChain.swapChainImageFormat == swapChainImageFormat  ;
 }
 private:
     void init();
     void createSwapChain();
  void createImageViews();
  void createColorResources();
  void createDepthResources();
  void createRenderPass();
  void createFramebuffers();
  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat swapChainImageFormat;
  VkFormat swapChainDepthFormat;
  VkExtent2D swapChainExtent;

  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  ShardDevice &device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;
  std::shared_ptr<ShardSwapChain> oldSwapChain;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;
  size_t currentFrame = 0;
};

}  // namespace shard
