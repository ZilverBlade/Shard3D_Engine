#include "../s3dtpch.h" 
#include "HUDLayer.hpp"
#include "../singleton.hpp"
#include "../scripts/dynamic_script_engine.hpp"
namespace Shard3D {
    HUDLayer::HUDLayer() : Layer("HUDLayer"){}

    HUDLayer::~HUDLayer() {}

    void HUDLayer::attach(VkRenderPass renderPass, LayerStack* layerStack) {
        window = Singleton::engineWindow.getGLFWwindow();
        Singleton::hudList.push_back(&hud);
        hudRenderSystem.create(renderPass);
    }

    void HUDLayer::detach() {
        hud.elements.clear();
        hudRenderSystem.destroy();
    }

    void HUDLayer::update(FrameInfo& frameInfo) {
        if (!Singleton::editorPreviewSettings.V_GUI) return;
        hudRenderSystem.render(frameInfo, hud);
        const uint64_t& id = hudRenderSystem.getSelectedID();
        if (id == 0) return;
        
        const auto& element = getSelectedElement();
        bool isPress = glfwGetMouseButton(Singleton::engineWindow.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        if (Singleton::activeLevel->simulationState == wb3d::PlayState::Simulating ||
            Singleton::activeLevel->simulationState == wb3d::PlayState::PausedRuntime) { 
            if (isPress)
                DynamicScriptEngine::hudScript().pressEvent(element.get(), frameInfo.frameTime);
            else
                DynamicScriptEngine::hudScript().hoverEvent(element.get(), frameInfo.frameTime);
        }
        hudRenderSystem.reset();
    }

    void HUDLayer::addElement(std::shared_ptr<HUDElement> element) {
        if (element->guid == 0) { SHARD3D_ERROR("You cannot create a HUD element with GUID 0"); return; }
        contextRefresh = true;
        hud.elements.emplace(element->guid, element);
    }

    std::shared_ptr<HUDElement> HUDLayer::getSelectedElement() {
        return hud.elements.find(hudRenderSystem.getSelectedID())->second;
    }

    void HUDLayer::rmvElement(std::shared_ptr<HUDElement> element) {
        hud.elements.erase(element->guid);
    }

    void HUDLayer::rmvElement(uint64_t id) {
        hud.elements.erase(id);
    }


}