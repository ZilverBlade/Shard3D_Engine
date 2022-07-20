#include "../s3dtpch.h" 
#include "ScreenLayer.hpp"
#include "../singleton.hpp"

namespace Shard3D {
    ScreenLayer::ScreenLayer() : Layer("ScreenLayer") {}

    ScreenLayer::~ScreenLayer() {}

    void ScreenLayer::attach(VkRenderPass renderPass, LayerStack* layerStack) {
      //  guiRenderSystem.create(renderPass);
    }

    void ScreenLayer::detach() {
      //  guiRenderSystem.destroy();
    }

    void ScreenLayer::update(FrameInfo& frameInfo) {
       // guiRenderSystem.renderSpecial(frameInfo, screenImage);
    }
}