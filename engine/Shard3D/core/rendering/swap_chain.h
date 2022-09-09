#pragma once

#include "../vulkan_api/device.h"

namespace Shard3D {

    class EngineSwapChain {
    public:
         static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    
         EngineSwapChain(EngineDevice &deviceRef, VkExtent2D windowExtent);
         EngineSwapChain(EngineDevice& deviceRef, VkExtent2D windowExtent, sPtr<EngineSwapChain> previous);
         ~EngineSwapChain();
    
         EngineSwapChain(const EngineSwapChain &) = delete;
         EngineSwapChain& operator=(const EngineSwapChain &) = delete;
         EngineSwapChain() = default;
    
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
 VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex, float additionalWaitTime);

 bool compareSwapFormats(const EngineSwapChain& swapChain) const {
     return swapChain.swapChainDepthFormat == swapChainDepthFormat  &&
            swapChain.swapChainImageFormat == swapChainImageFormat  ;
 }
 private:
     void init();
     void createSwapChain();
  void createImageViews();
  //void createColorResources();
  void createDepthResources();
  void createRenderPass();
  void createFramebuffers();
  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats);


  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat swapChainImageFormat;
  VkFormat swapChainDepthFormat;
  VkExtent2D swapChainExtent;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  EngineDevice &device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;
  sPtr<EngineSwapChain> oldSwapChain;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;
  size_t currentFrame = 0;
};

}  // namespace Shard3D
