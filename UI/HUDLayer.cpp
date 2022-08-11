#include "../s3dtpch.h" 
#include "HUDLayer.hpp"
#include "../singleton.hpp"
#include "../scripts/dynamic_script_engine.hpp"
#include "../utils/stats_timing.h"
#include "../input.h"
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
        SHARD3D_STAT_RECORD();
        hudRenderSystem.render(frameInfo, hud);
        const uint64_t& id = hudRenderSystem.getSelectedID();
        if (id == 0) { SHARD3D_STAT_RECORD_END({ "HUD", std::string("Layer " + std::to_string(layer)) }); return; }
        const auto& element = getSelectedElement();
        bool isPress = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);

        if (Singleton::activeLevel->simulationState == wb3d::PlayState::Simulating ||
            Singleton::activeLevel->simulationState == wb3d::PlayState::PausedRuntime) {
            if (isPress)
                DynamicScriptEngine::hudScript().pressEvent(element.get(), frameInfo.frameTime);
            else
                DynamicScriptEngine::hudScript().hoverEvent(element.get(), frameInfo.frameTime);
        }
  
        SHARD3D_EVENT_BIND_HANDLER(HUDLayer::eventEvent);
        hudRenderSystem.reset();
        SHARD3D_STAT_RECORD_END({ "HUD", std::string("Layer " + std::to_string(layer))});
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

    void HUDLayer::eventEvent(Event& e) {
        SHARD3D_EVENT_CREATE_DISPATCHER(e);
        SHARD3D_EVENT_DISPATCH(MouseButtonDownEvent, HUDLayer::mouseButtonDownEvent);
    }

    bool HUDLayer::mouseButtonDownEvent(MouseButtonDownEvent& e) {
        bool isClick = e.getButtonCode() == GLFW_MOUSE_BUTTON_LEFT;

        if (Singleton::activeLevel->simulationState == wb3d::PlayState::Simulating ||
            Singleton::activeLevel->simulationState == wb3d::PlayState::PausedRuntime && isClick) {
            const uint64_t& id = hudRenderSystem.getSelectedID();
            if (id == 0) return false;
            const auto& element = getSelectedElement();
                DynamicScriptEngine::hudScript().clickEvent(element.get());
        }
        return false;
    }


}