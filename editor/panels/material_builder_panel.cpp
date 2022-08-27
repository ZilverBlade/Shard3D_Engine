#include "material_builder_panel.h"

#include <imgui.h>
#include <Shard3D/core/asset/assetmgr.h>
#include <glm/gtc/type_ptr.hpp>
namespace Shard3D {
	MaterialBuilderPanel::MaterialBuilderPanel() : currentAsset(ENGINE_ERRMAT ENGINE_ASSET_SUFFIX) {
        currentItem = ResourceHandler::retrieveSurfaceMaterial(currentAsset);
    }
	MaterialBuilderPanel::~MaterialBuilderPanel() {}

	void MaterialBuilderPanel::render() {
		ImGui::Begin("Material Builder");
       if (ImGui::BeginCombo("##listofmaterialscombo", currentItem->materialTag.c_str())) {
         
           ImGui::EndCombo();
       }

		if (ImGui::BeginDragDropTarget())
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.SMAT")) {
				currentAsset = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
				ResourceHandler::loadSurfaceMaterial(currentAsset);
				currentItem = ResourceHandler::retrieveSurfaceMaterial(currentAsset);
			}

		ImGui::Text(typeid(*currentItem).name());
		if (typeid(*currentItem).raw_name() == typeid(SurfaceMaterial_ShadedOpaque).raw_name()) {
			auto* surfaceMaterial = reinterpret_cast<SurfaceMaterial_ShadedOpaque*>(currentItem.get());
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
			ImGui::DragFloat("Metallic", &surfaceMaterial->metallic, 0.01f, 0.f, 1.f);
			{
				auto& tag = surfaceMaterial->metallicTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Metallic Map", tagBuffer, 256)) {
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
			int cullMode = static_cast<int>(surfaceMaterial->drawData.culling);
			ImGui::Combo("Cull Mode", &cullMode, "VK_CULL_MODE_NONE\0VK_CULL_MODE_FRONT_BIT\0VK_CULL_MODE_BACK_BIT\0");
			surfaceMaterial->drawData.culling = cullMode;	
		}

		if (typeid(*currentItem).raw_name() == typeid(SurfaceMaterial_ShadedMasked).raw_name()) {
			auto* surfaceMaterial = reinterpret_cast<SurfaceMaterial_ShadedMasked*>(currentItem.get());
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
			ImGui::DragFloat("Metallic", &surfaceMaterial->metallic, 0.01f, 0.f, 1.f);
			{
				auto& tag = surfaceMaterial->metallicTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Metallic Map", tagBuffer, 256)) {
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
			{
				auto& tag = surfaceMaterial->maskTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Opacity Mask", tagBuffer, 256)) {
					//	tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->maskTex = AssetID(surfaceMaterial->maskTex.getFile());
					}
			}
			int cullMode = static_cast<int>(surfaceMaterial->drawData.culling);
			ImGui::Combo("Cull Mode", &cullMode, "VK_CULL_MODE_NONE\0VK_CULL_MODE_FRONT_BIT\0VK_CULL_MODE_BACK_BIT\0");
			surfaceMaterial->drawData.culling = cullMode;
		}

		if (typeid(*currentItem).raw_name() == typeid(SurfaceMaterial_ShadedTranslucent).raw_name()) {
			auto* surfaceMaterial = reinterpret_cast<SurfaceMaterial_ShadedTranslucent*>(currentItem.get());
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
			ImGui::DragFloat("Clarity", &surfaceMaterial->metallic, 0.01f, 0.f, 1.f);
			{
				auto& tag = surfaceMaterial->metallicTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Clarity Map", tagBuffer, 256)) {
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
			ImGui::DragFloat("Opacity", &surfaceMaterial->opacity, 0.01f, 0.f, 1.f);
			{
				auto& tag = surfaceMaterial->opacityTex.getFileRef();
				char tagBuffer[256];
				memset(tagBuffer, 0, 256);
				strncpy(tagBuffer, tag.c_str(), 256);
				if (ImGui::InputText("Opacity Map", tagBuffer, 256)) {
					//	tag = std::string(tagBuffer);
				}
				if (ImGui::BeginDragDropTarget())
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.TEX")) {
						tag = std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data);
						surfaceMaterial->opacityTex = AssetID(surfaceMaterial->opacityTex.getFile());
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

	/*

const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO", "PPPP", "QQQQQQQQQQ", "RRR", "SSSS" };
static const char* current_item = NULL;

if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
{
	for (int n = 0; n < IM_ARRAYSIZE(items); n++)
	{
		bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
		if (ImGui::Selectable(items[n], is_selected)
			current_item = items[n];
		if (is_selected)
			ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
	}
	ImGui::EndCombo();
}


	*/
}