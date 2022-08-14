
#include "screen_layer.h"

namespace Shard3D {
    ScreenLayer::ScreenLayer() : Layer("ScreenLayer") {}

    ScreenLayer::~ScreenLayer() {}

    void ScreenLayer::attach(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, LayerStack* layerStack) {
      //  guiRenderSystem.create(renderPass);
    }

    void ScreenLayer::detach() {
      //  guiRenderSystem.destroy();
    }

    void ScreenLayer::update(FrameInfo& frameInfo) {
       // guiRenderSystem.renderSpecial(frameInfo, screenImage);
    }
}