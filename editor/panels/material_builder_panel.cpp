#include "material_builder_panel.h"

#include <imgui.h>
#include <Shard3D/core/asset/assetmgr.h>
#include <glm/gtc/type_ptr.hpp>
namespace Shard3D {
	MaterialBuilderPanel::MaterialBuilderPanel() : currentAsset(ENGINE_ERRMAT ENGINE_ASSET_SUFFIX) {
        currentItem = ResourceHandler::retrieveSurfaceMaterial(currentAsset);
    }
	MaterialBuilderPanel::~MaterialBuilderPanel() {}

	static std::string blendModeToString(SurfaceMaterialBlendMode_T blendMode) {
		std::string blendMode_str;
		if (blendMode & SurfaceMaterialBlendModeTranslucent)
			blendMode_str = std::string(blendMode_str + "Translucent");
		if (blendMode & SurfaceMaterialBlendModeMasked)
			blendMode_str = std::string(blendMode_str + "Masked");
		if (!(blendMode & SurfaceMaterialBlendModeTranslucent) || !(blendMode & SurfaceMaterialBlendModeMasked))
			blendMode_str = "Opaque";
		return blendMode_str;
	}

	static SurfaceMaterialBlendMode_T stringToBlendMode(const char* blendMode) {
		SurfaceMaterialBlendMode_T blendMode_flags = SurfaceMaterialBlendModeOpaque;
		if (strstr(blendMode, "Translucent") != nullptr)
			blendMode_flags |= SurfaceMaterialBlendModeTranslucent;
		if (strstr(blendMode, "Masked") != nullptr)
			blendMode_flags |= SurfaceMaterialBlendModeMasked;
		return blendMode_flags;
	}

	void MaterialBuilderPanel::render() {
		ImGui::Begin("Material Builder");
		if (ImGui::BeginCombo("##listofmaterialscombo", currentItem->materialTag.c_str())) ImGui::EndCombo();
		
		if (ImGui::BeginDragDropTarget())
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.SMAT")) {
				currentAsset = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
				ResourceHandler::loadSurfaceMaterial(currentAsset);
				currentItem = ResourceHandler::retrieveSurfaceMaterial(currentAsset);
			}


		const char* items[] = { "Opaque", "Masked", "Translucent", "TranslucentMasked" };
		const char* current_item = items[currentItem->getBlendMode()];
		
		if (ImGui::BeginCombo("##blendopcombo", current_item)) { // The second parameter is the label previewed before opening the combo.
			for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
				bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected)) {
					current_item = items[n];
					currentItem->setBlendMode(n);
					ResourceHandler::rebuildSurfaceMaterial(currentItem);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}

	
		ImGui::Text(typeid(*currentItem).name());
		if (typeid(*currentItem).raw_name() == typeid(SurfaceMaterial_Shaded).raw_name()) {
			auto* surfaceMaterial = reinterpret_cast<SurfaceMaterial_Shaded*>(currentItem.get());
			ImGui::ColorEdit3("Diffuse", glm::value_ptr(surfaceMaterial->diffuseColor));
			{
				auto& tag = surfaceMaterial->diffuseTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Diffuse Texture", tagBuffer, 256)) {
					//tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->diffuseTex = AssetID(surfaceMaterial->diffuseTex.getFile());
					}
			}
			ImGui::DragFloat("Specular", &surfaceMaterial->specular, 0.01f, 0.f, 1.f);
			{
				auto& tag = surfaceMaterial->specularTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Specular Map", tagBuffer, 256)) {
					//tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->specularTex = AssetID(surfaceMaterial->specularTex.getFile());
					}
			}
			ImGui::DragFloat("Shininess", &surfaceMaterial->shininess, 0.01f, 0.f, 1.f);
			{
				auto& tag = surfaceMaterial->shininessTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Shininess Map", tagBuffer, 256)) {
				//	tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->shininessTex = AssetID(surfaceMaterial->shininessTex.getFile());
					}
			}
			ImGui::DragFloat(!(surfaceMaterial->getBlendMode() & SurfaceMaterialBlendModeTranslucent) ? "Metallic" : "Clarity", &surfaceMaterial->metallic, 0.01f, 0.f, 1.f);
			{
				auto& tag = surfaceMaterial->metallicTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText(!(surfaceMaterial->getBlendMode() & SurfaceMaterialBlendModeTranslucent) ? "Metallic Map" : "Clarity Map", tagBuffer, 256)) {
				//	tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->metallicTex = AssetID(surfaceMaterial->metallicTex.getFile());
					}
			}
			{
				auto& tag = surfaceMaterial->normalTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Normal Map", tagBuffer, 256)) {
				//	tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->normalTex = AssetID(surfaceMaterial->normalTex.getFile());
					}
			}
			if (surfaceMaterial->getBlendMode() & SurfaceMaterialBlendModeMasked)
			{
				auto& tag = surfaceMaterial->maskedInfo->maskTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Opacity Mask", tagBuffer, 256)) {
					//	tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->maskedInfo->maskTex = AssetID(surfaceMaterial->maskedInfo->maskTex.getFile());
					}
			}
			if (surfaceMaterial->getBlendMode() & SurfaceMaterialBlendModeTranslucent)
			{
				ImGui::DragFloat("Opacity", &surfaceMaterial->translucentInfo->opacity, 0.01f, 0.f, 1.f);
				{
					auto& tag = surfaceMaterial->translucentInfo->opacityTex.getFileRef();
					char tagBuffer[256];
					memset(tagBuffer, 0, 256);
					strncpy(tagBuffer, tag.c_str(), 256);
					if (ImGui::InputText("Opacity Map", tagBuffer, 256)) {
						//	tag = std::string(tagBuffer);
					}
					if (ImGui::BeginDragDropTarget())
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
							tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
							surfaceMaterial->translucentInfo->opacityTex = AssetID(surfaceMaterial->translucentInfo->opacityTex.getFile());
						}
				}
			}

			int cullMode = static_cast<int>(surfaceMaterial->drawData.culling);
			ImGui::Combo("Cull Mode", &cullMode, "VK_CULL_MODE_NONE\0VK_CULL_MODE_FRONT_BIT\0VK_CULL_MODE_BACK_BIT\0");
			surfaceMaterial->drawData.culling = cullMode;	
		}
		
		if (ImGui::Button("Rebuild")) {
			SHARD3D_INFO("Rebuilding material...");
			ResourceHandler::rebuildSurfaceMaterial(currentItem);
		}
		if (ImGui::Button("Save")) {
			AssetManager::createMaterial(currentAsset.getFile(), currentItem);
		}
		ImGui::End();
	}
}