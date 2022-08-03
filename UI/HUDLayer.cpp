#include "../s3dtpch.h" 
#include "HUDLayer.hpp"
#include "../singleton.hpp"

namespace Shard3D {
    HUDLayer::HUDLayer() : Layer("HUDLayer"){}

    HUDLayer::~HUDLayer() {}

    void HUDLayer::attach(VkRenderPass renderPass, LayerStack* layerStack) {
        window = Singleton::engineWindow.getGLFWwindow();
        hudRenderSystem.create(renderPass);
    }

    void HUDLayer::detach() {
        hud.elements.clear();
        hudRenderSystem.destroy();
    }

    void HUDLayer::update(FrameInfo& frameInfo) {
        if (!Singleton::editorPreviewSettings.V_GUI) return;
        hudRenderSystem.render(frameInfo, hud);
        if (Singleton::activeLevel->simulationState == wb3d::PlayState::Simulating || 
            Singleton::activeLevel->simulationState == wb3d::PlayState::PausedRuntime)
        if (glfwGetMouseButton(Singleton::engineWindow.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {     
            if (getSelectedID() == 0) return;
            getSelectedElement()->pressEventCallback();
        }
        hudRenderSystem.reset();
    }

    void HUDLayer::addElement(std::shared_ptr<HUD::Element> element) {
        if (element->guid == 0) { SHARD3D_ERROR("You cannot create a HUD element with GUID 0"); return; }
        contextRefresh = true;
        hud.elements.emplace(element->guid, element);
    }

    uint64_t HUDLayer::getSelectedID() {
        auto* v = hudRenderSystem.pickBuffer->getMappedMemory();
        auto u = static_cast<uint64_t*>(v);
        return *u;
    }

    std::shared_ptr<HUD::Element> HUDLayer::getSelectedElement() {
        SHARD3D_LOG(getSelectedID());
        return hud.elements.find(getSelectedID())->second;
    }

    void HUDLayer::rmvElement(std::shared_ptr<HUD::Element> element) {
        hud.elements.erase(element->guid);
    }

    void HUDLayer::rmvElement(uint64_t id) {
        hud.elements.erase(id);
    }


}