
#include "hud_layer.h"

#include "../../scripting/dynamic_script_engine.h"
#include "../misc/graphics_settings.h"
#include "../../workarounds.h"
#include "../../core.h"

namespace Shard3D {
    HUDLayer::HUDLayer() : Layer("HUDLayer"){}

    HUDLayer::~HUDLayer() {}

    void HUDLayer::attach(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass, LayerStack* layerStack) {
        TEMPORARY::hudList.push_back(&hud);
        hudRenderSystem.create(dvc, wnd, renderPass);
    }

    void HUDLayer::detach() {
        hud.elements.clear();
        hudRenderSystem.destroy();
    }

     void HUDLayer::update(FrameInfo& frameInfo) {
        if (!GraphicsSettings::editorPreview.V_GUI) return;
        SHARD3D_STAT_RECORD();
        hudRenderSystem.render(frameInfo, hud);
        const uint64_t& id = hudRenderSystem.getSelectedID();
        if (id == 0) { SHARD3D_STAT_RECORD_END({ "HUD", std::string("Layer " + std::to_string(layer)) }); return; }
        const auto& element = getSelectedElement();
        bool isPress = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);

        if (frameInfo.activeLevel->simulationState == ECS::PlayState::Simulating ||
            frameInfo.activeLevel->simulationState == ECS::PlayState::PausedRuntime) {
            if (isPress)
                DynamicScriptEngine::hudScript().pressEvent(element.get(), frameInfo.frameTime);
            else
                DynamicScriptEngine::hudScript().hoverEvent(element.get(), frameInfo.frameTime);
        }
  
      //  SHARD3D_EVENT_BIND_HANDLER_PTR(windowHUDLayer::eventEvent);
        hudRenderSystem.reset();
        SHARD3D_STAT_RECORD_END({ "HUD", std::string("Layer " + std::to_string(layer))});
    }

    void HUDLayer::addElement(sPtr<HUDElement> element) {
        if (element->guid == 0) { SHARD3D_ERROR("You cannot create a HUD element with UUID 0"); return; }
        contextRefresh = true;
        hud.elements.emplace(element->guid, element);
    }

    sPtr<HUDElement> HUDLayer::getSelectedElement() {
        return hud.elements.find(hudRenderSystem.getSelectedID())->second;
    }

    void HUDLayer::rmvElement(sPtr<HUDElement> element) {
        hud.elements.erase(element->guid);
    }

    void HUDLayer::rmvElement(uint64_t id) {
        hud.elements.erase(id);
    }

    void HUDLayer::eventEvent(Events::Event& e) {
        SHARD3D_EVENT_CREATE_DISPATCHER(e);
        SHARD3D_EVENT_DISPATCH(Events::MouseButtonDownEvent, HUDLayer::mouseButtonDownEvent);
    }

    bool HUDLayer::mouseButtonDownEvent(Events::MouseButtonDownEvent& e) {
        bool isClick = e.getButtonCode() == GLFW_MOUSE_BUTTON_LEFT;

        //if (Singleton::activeLevel->simulationState == ECS::PlayState::Simulating ||
        //    Singleton::activeLevel->simulationState == ECS::PlayState::PausedRuntime && isClick) {
        //    const uint64_t& id = hudRenderSystem.getSelectedID();
        //    if (id == 0) return false;
        //    const auto& element = getSelectedElement();
        //        DynamicScriptEngine::hudScript().clickEvent(element.get());
        //}
        return false;
    }


}