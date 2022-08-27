#include "default_component_panels.h"
#include "../imgui/imgui_implementation.h"
#include <imgui_internal.h>
namespace Shard3D {
	static void drawImageButtonWithText(ImTextureID textureID, const char* label, const ImVec2& size, size_t payloadHashCode) {
		ImGui::Image(textureID, size); 
		ImGui::SameLine();
		ImGui::Selectable(label, false, ImGuiSelectableFlags_SelectOnClick);
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("SHARD3D.COMPONENTS.DROP", &payloadHashCode, sizeof(size_t), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}
	}

	DefaultComponentPanels::DefaultComponentPanels() {
		createIcons();
	}
	DefaultComponentPanels::~DefaultComponentPanels() {
	}

	void DefaultComponentPanels::createIcons() {
		{
			auto& img = _special_assets::_editor_icons.at("component.light.point");
			icons.pointlight = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("component.light.spot");
			icons.spotlight = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("component.light.directional");
			icons.sun = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("component.camera");
			icons.camera = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("component.audio");
			icons.cube = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("component.audio");
			icons.sphere = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	void DefaultComponentPanels::render() {
		ImGui::Begin("Basic Actors");
		ImGui::BeginListBox("##actorlistboxpanel", ImGui::GetContentRegionAvail());
		drawImageButtonWithText(icons.pointlight, "Pointlight Actor", { 32.f, 32.f }, typeid(Components::PointlightComponent).hash_code());
		drawImageButtonWithText(icons.spotlight, "Spotlight Actor", { 32.f, 32.f }, typeid(Components::SpotlightComponent).hash_code());
		drawImageButtonWithText(icons.camera, "Camera Actor", { 32.f, 32.f }, typeid(Components::CameraComponent).hash_code());
		drawImageButtonWithText(icons.cube, "Cube (Static)", { 32.f, 32.f }, 32325235);
		drawImageButtonWithText(icons.sphere, "Sphere (Static)", { 32.f, 32.f }, 3232);
		ImGui::EndListBox();
		
		ImGui::End();
	}

	

}
