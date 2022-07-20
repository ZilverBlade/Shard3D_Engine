#include "../s3dtpch.h" 
#include "GUILayer.hpp"
#include "../singleton.hpp"

namespace Shard3D {
    GUILayer::GUILayer() : Layer("GUILayer"){}

    GUILayer::~GUILayer() {}

    void GUILayer::attach(VkRenderPass renderPass, LayerStack* layerStack) {
        window = Singleton::engineWindow.getGLFWwindow();
        guiRenderSystem.create(renderPass);
    }

    void GUILayer::detach() {
        guiElements.elementsGUI.clear();
        guiRenderSystem.destroy();
    }

    void GUILayer::update(FrameInfo& frameInfo) {
        guiRenderSystem.render(frameInfo, guiElements);
        if (Singleton::activeLevel->simulationState == wb3d::PlayState::Simulating) // only react to mouse when not playing
        if (glfwGetMouseButton(Singleton::engineWindow.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {     
            if (getSelectedID() == 0) return;
            getSelectedElement()->pressEventCallback();
        }
    }

    void GUILayer::addElement(std::shared_ptr<GUI::Element> element) {
        if (element->guid == 0) { SHARD3D_ERROR("You cannot create a GUI element with GUID 0"); return; }
        guiElements.elementsGUI.emplace(element->guid, element);
    }

    uint64_t GUILayer::getSelectedID() {
        auto* v = guiRenderSystem.pickBuffer->getMappedMemory();
        auto u = static_cast<uint64_t*>(v);
        return *u;
    }

    std::shared_ptr<GUI::Element> GUILayer::getSelectedElement() {
        
        return guiElements.elementsGUI.find(getSelectedID())->second;
    }

    void GUILayer::rmvElement(std::shared_ptr<GUI::Element> element) {
        guiElements.elementsGUI.erase(element->guid);
    }

    void GUILayer::rmvElement(uint64_t id) {
        guiElements.elementsGUI.erase(id);
    }


}