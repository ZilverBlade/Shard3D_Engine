#include "../s3dtpch.h" 
#include "GUILayer.hpp"
#include "../singleton.hpp"

namespace Shard3D {
    GUILayer::GUILayer() : Layer("ImGuiLayer") {}

    GUILayer::~GUILayer() {}

    void GUILayer::attach(VkRenderPass renderPass) {
        guiRenderSystem.create(renderPass);
    }

    void GUILayer::detach() {
        guiRenderSystem.destroy();
    }

    void GUILayer::update(FrameInfo frameInfo) {
        guiRenderSystem.render(frameInfo, guiElements);
    }

    void GUILayer::addElement(GUI::Element element) {
        guiElements.elementsGUI.push_back(element);
    }

    void GUILayer::rmvElement(GUI::Element element) {
       // std::vector<GUI::Element>::iterator itr = 
       //     std::find(guiElements.elementsGUI.begin(), guiElements.elementsGUI.end(), element);
       //
       // guiElements.elementsGUI.erase(guiElements.elementsGUI.begin() + 
       //     std::distance(guiElements.elementsGUI.begin(), itr));
    }

    void GUILayer::rmvElement(size_t index) {
        guiElements.elementsGUI.erase(guiElements.elementsGUI.begin() + index);
    }


}