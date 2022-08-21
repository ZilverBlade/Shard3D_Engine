#include "../s3dpch.h"
#include <imgui.h>
#include "level_peekers.h"
#include "../ecs.h"
#include "../core/asset/assetmgr.h"
#include <imgui_internal.h>

namespace Shard3D {

	float deltaTimeFromLastSecond{};
	float timeSinceLastSecond{};
	static bool liveRebuild;

	LevelPeekingPanel::LevelPeekingPanel(const sPtr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPeekingPanel::~LevelPeekingPanel() { context = nullptr; }

	void LevelPeekingPanel::setContext(const sPtr<Level>& levelContext) { context = levelContext; }
	void LevelPeekingPanel::destroyContext() { context = {}; }

	void LevelPeekingPanel::render(FrameInfo& frameInfo) {
		SHARD3D_ASSERT(context != nullptr && "Context not provided!");

		timeSinceLastSecond += frameInfo.frameTime;

		ImGui::Begin("Level Peeker");
		ImGui::Checkbox("Actor Inspector", &actorInspector);
		ImGui::Separator();
		ImGui::Checkbox("LOD Inspector", &lodInspector);
		ImGui::Checkbox("Texture Inspector", &textureInspector);
		ImGui::Separator();
		ImGui::Checkbox("Material Inspector", &materialInspector);
		ImGui::Separator();
		ImGui::Checkbox("Miscellaneous Inspector", &miscInspector);
		ImGui::End();

		if (!actorInspector) peekActorInspector();
		if (lodInspector) peekLODInspector();
		if (textureInspector) peekTextureInspector();
		if (materialInspector) peekMaterialInspector();
		if (miscInspector) peekMisc();
	}

	static void drawColorControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float stepVal = 0.01f, float columnWidth = 100.f) {
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
		float lineHeight = ENGINE_FONT_SIZE + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.2f, 0.2f, 1.f));
		if (ImGui::Button("R", buttonSize)) values.x = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##R", &values.x, stepVal, 0.f, 1.f); ImGui::PopItemWidth(); ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.5f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.7f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.7f, 0.3f, 1.f));
		if (ImGui::Button("G", buttonSize)) values.z = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##G", &values.y, stepVal, 0.f, 1.f); ImGui::PopItemWidth(); ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.7f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.9f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.9f, 1.f));
		if (ImGui::Button("B", buttonSize)) values.y = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine(); ImGui::DragFloat("##B", &values.z, stepVal, 0.f, 1.f); ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}


	void LevelPeekingPanel::peekMaterialInspector() {
		ImGui::Begin("Material Inspector");
		for (auto& i : AssetManager::getSurfaceMaterialAssets()) {
			std::string text = i.second->materialTag + " (" + i.first + ")";
			if (ImGui::TreeNode(text.c_str())) {
				if (typeid(*i.second).raw_name() == typeid(SurfaceMaterial_ShadedOpaque).raw_name()) {
					auto* surfaceMaterial = reinterpret_cast<SurfaceMaterial_ShadedOpaque*>(i.second.get());
					drawColorControl("Diffuse", surfaceMaterial->diffuseColor, 1.f, 0.01f);
					{
						auto& tag = surfaceMaterial->diffuseTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Diffuse Texture", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Specular", &surfaceMaterial->specular, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->specularTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Specular Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Shininess", &surfaceMaterial->shininess, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->shininessTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Shininess Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Metallic", &surfaceMaterial->metallic, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->metallicTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Metallic Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					{
						auto& tag = surfaceMaterial->normalTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Normal Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					int cullMode = static_cast<int>(surfaceMaterial->drawData.culling);
					ImGui::Combo("Cull Mode", &cullMode, "VK_CULL_MODE_NONE\0VK_CULL_MODE_FRONT_BIT\0VK_CULL_MODE_BACK_BIT\0");
					surfaceMaterial->drawData.culling = cullMode;
					if (ImGui::Button("Switch to masked")) {
						const std::string& name = i.second->materialTag;
						i.second = make_sPtr<SurfaceMaterial_ShadedMasked>();
						i.second->materialTag = name;
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					if (ImGui::Button("Switch to translucent")) {
						const std::string& name = i.second->materialTag;
						i.second = make_sPtr<SurfaceMaterial_ShadedTranslucent>();
						i.second->materialTag = name;
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					ImGui::Checkbox("Live Rebuild", &liveRebuild);
					if (ImGui::Button("Rebuild Material")) {
						AssetManager::emplaceTexture(surfaceMaterial->diffuseTex);
						AssetManager::emplaceTexture(surfaceMaterial->specularTex);
						AssetManager::emplaceTexture(surfaceMaterial->shininessTex);
						AssetManager::emplaceTexture(surfaceMaterial->metallicTex);
						AssetManager::emplaceTexture(surfaceMaterial->normalTex);
						AssetManager::emplaceMaterial(i.second, i.first);
					}										
					if (liveRebuild) {
						if (timeSinceLastSecond > 0.5f) { timeSinceLastSecond = 0.f;  
							AssetManager::emplaceTexture(surfaceMaterial->diffuseTex);
							AssetManager::emplaceTexture(surfaceMaterial->specularTex);
							AssetManager::emplaceTexture(surfaceMaterial->shininessTex);
							AssetManager::emplaceTexture(surfaceMaterial->metallicTex);
							AssetManager::emplaceTexture(surfaceMaterial->normalTex);
							AssetManager::emplaceMaterial(i.second, i.first);
						}
					}
				}

				if (typeid(*i.second).raw_name() == typeid(SurfaceMaterial_ShadedMasked).raw_name()) {
					auto* surfaceMaterial = reinterpret_cast<SurfaceMaterial_ShadedMasked*>(i.second.get());
					drawColorControl("Diffuse", surfaceMaterial->diffuseColor, 1.f, 0.01f);
					{
						auto& tag = surfaceMaterial->diffuseTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Diffuse Texture", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Specular", &surfaceMaterial->specular, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->specularTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Specular Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Shininess", &surfaceMaterial->shininess, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->shininessTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Shininess Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Metallic", &surfaceMaterial->metallic, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->metallicTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Metallic Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					{
						auto& tag = surfaceMaterial->normalTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Normal Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					{
						auto& tag = surfaceMaterial->maskTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Alpha Mask", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					int cullMode = static_cast<int>(surfaceMaterial->drawData.culling);
					ImGui::Combo("Cull Mode", &cullMode, "VK_CULL_MODE_NONE\0VK_CULL_MODE_FRONT_BIT\0VK_CULL_MODE_BACK_BIT\0");
					surfaceMaterial->drawData.culling = cullMode;
					if (ImGui::Button("Switch to opaque")) {
						const std::string& name = i.second->materialTag;
						i.second = make_sPtr<SurfaceMaterial_ShadedOpaque>();
						i.second->materialTag = name;
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					if (ImGui::Button("Switch to translucent")) {
						const std::string& name = i.second->materialTag;
						i.second = make_sPtr<SurfaceMaterial_ShadedTranslucent>();
						i.second->materialTag = name;
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					ImGui::Checkbox("Live Rebuild", &liveRebuild);
					if (ImGui::Button("Rebuild Material")) {
						AssetManager::emplaceTexture(surfaceMaterial->diffuseTex);
						AssetManager::emplaceTexture(surfaceMaterial->specularTex);
						AssetManager::emplaceTexture(surfaceMaterial->shininessTex);
						AssetManager::emplaceTexture(surfaceMaterial->metallicTex);
						AssetManager::emplaceTexture(surfaceMaterial->normalTex);
						AssetManager::emplaceTexture(surfaceMaterial->maskTex);
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					if (liveRebuild) {
						if (timeSinceLastSecond > 0.5f) {
							timeSinceLastSecond = 0.f;
							AssetManager::emplaceTexture(surfaceMaterial->diffuseTex);
							AssetManager::emplaceTexture(surfaceMaterial->specularTex);
							AssetManager::emplaceTexture(surfaceMaterial->shininessTex);
							AssetManager::emplaceTexture(surfaceMaterial->metallicTex);
							AssetManager::emplaceTexture(surfaceMaterial->normalTex);
							AssetManager::emplaceTexture(surfaceMaterial->maskTex);
							AssetManager::emplaceMaterial(i.second, i.first);
						}
					}
				}

				if (typeid(*i.second).raw_name() == typeid(SurfaceMaterial_ShadedTranslucent).raw_name()) {
					auto* surfaceMaterial = reinterpret_cast<SurfaceMaterial_ShadedTranslucent*>(i.second.get());
					drawColorControl("Diffuse", surfaceMaterial->diffuseColor, 1.f, 0.01f);
					{
						auto& tag = surfaceMaterial->diffuseTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Diffuse Texture", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Specular", &surfaceMaterial->specular, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->specularTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Specular Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Shininess", &surfaceMaterial->shininess, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->shininessTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Shininess Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Clarity", &surfaceMaterial->clarity, 0.01f, 0.f, 1.f);
					{
						auto& tag = surfaceMaterial->clarityTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Clarity Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					{
						auto& tag = surfaceMaterial->normalTex;
						char tagBuffer[256];
						memset(tagBuffer, 0, 256);
						strncpy(tagBuffer, tag.c_str(), 256);
						if (ImGui::InputText("Normal Map", tagBuffer, 256)) {
							tag = std::string(tagBuffer);
						}
					}
					ImGui::DragFloat("Opacity", &surfaceMaterial->opacity, 0.01f, 0.f, 1.f);
					int cullMode = static_cast<int>(surfaceMaterial->drawData.culling);
					ImGui::Combo("Cull Mode", &cullMode, "VK_CULL_MODE_NONE\0VK_CULL_MODE_FRONT_BIT\0VK_CULL_MODE_BACK_BIT\0");
					surfaceMaterial->drawData.culling = cullMode;
					if (ImGui::Button("Switch to masked")) {
						const std::string& name = i.second->materialTag;
						i.second = make_sPtr<SurfaceMaterial_ShadedMasked>();
						i.second->materialTag = name;
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					if (ImGui::Button("Switch to opaque")) {
						const std::string& name = i.second->materialTag;
						i.second = make_sPtr<SurfaceMaterial_ShadedOpaque>();
						i.second->materialTag = name;
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					ImGui::Checkbox("Live Rebuild", &liveRebuild);
					if (ImGui::Button("Rebuild Material")) {
						AssetManager::emplaceTexture(surfaceMaterial->diffuseTex);
						AssetManager::emplaceTexture(surfaceMaterial->specularTex);
						AssetManager::emplaceTexture(surfaceMaterial->shininessTex);
						AssetManager::emplaceTexture(surfaceMaterial->clarityTex);
						AssetManager::emplaceTexture(surfaceMaterial->normalTex);
						AssetManager::emplaceMaterial(i.second, i.first);
					}
					if (liveRebuild) {
						if (timeSinceLastSecond > 0.5f) {
							timeSinceLastSecond = 0.f;
							AssetManager::emplaceTexture(surfaceMaterial->diffuseTex);
							AssetManager::emplaceTexture(surfaceMaterial->specularTex);
							AssetManager::emplaceTexture(surfaceMaterial->shininessTex);
							AssetManager::emplaceTexture(surfaceMaterial->clarityTex);
							AssetManager::emplaceTexture(surfaceMaterial->normalTex);
							AssetManager::emplaceMaterial(i.second, i.first);
						}
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void LevelPeekingPanel::peekActorInspector() {
		ImGui::Begin("Actor Inspector");
		context->registry.each([&](auto actorGUID) {
			ECS::Actor actor{ actorGUID, context.get() };

			std::string text = actor.getTag() + " (" + std::to_string((uint64_t)actor.getUUID()) + ")" 
				+ " handle (" + std::to_string((uint32_t)actor.actorHandle) + ")"
				+ " p_handle (" + std::to_string((uint32_t)context->getParent(actor).actorHandle) + ")";
			if (actor.isInvalid())
				ImGui::TextColored({1.f, 0.f, 1.f, 1.f}, text.c_str());
			else
				ImGui::Text(text.c_str());
		});
		ImGui::End();
	}

	void LevelPeekingPanel::peekTextureInspector() {
		ImGui::Begin("Texture Inspector");
		ImGui::Text((std::string("Texture count: " + std::to_string(AssetManager::getTextureAssets().size())).c_str()));
		if (ImGui::Button("Force delete all textures (dangerous)")) {
			if (MessageDialogs::show(std::string("Deletion of all assets can be dangerous if assets are in use.\nUnexpected crashes may happen if one of these assets is in use.\nAre you sure you want to proceed ?").c_str(), "Caution!", MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTYESNO) == MessageDialogs::RESYES) {
				AssetManager::clearTextureAssets();
			}
		}
		for (const auto& tex : AssetManager::getTextureAssets())
		ImGui::Text(std::string(tex.first).c_str());
		ImGui::End();
	}
	void LevelPeekingPanel::peekLODInspector() {
		ImGui::Begin("Mesh & LOD Inspector");
		ImGui::Text((std::string("Mesh count: " + std::to_string(AssetManager::getMeshAssets().size())).c_str()));
		if (ImGui::Button("Force delete all models (dangerous)")) {
			if (MessageDialogs::show(std::string("Deletion of all assets can be dangerous if assets are in use.\nUnexpected crashes may happen if one of these assets is in use.\nAre you sure you want to proceed ?").c_str(), "Caution!", MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTYESNO) == MessageDialogs::RESYES) {
				AssetManager::clearMeshAssets();
			}
		}

		for (const auto& model : AssetManager::getMeshAssets())
		ImGui::Text(std::string(model.first).c_str());
		ImGui::End();
	}

	void LevelPeekingPanel::peekMisc() {
		ImGui::Begin("Misc Inspector");
		ImGui::Text(std::string("Active level ("+ context->name+") ptr: 0x" + std::to_string((int)context.get())).c_str());

		ImGui::Text(std::string("Possessed camera actor: " + context->getPossessedCameraActor().getTag() + " (0x" + std::to_string((int)&context->getPossessedCamera()) + ")").c_str());
#if ENSET_ALLOW_PREVIEW_CAMERA
		ImGui::Text(std::string("Previewing camera: " + Singleton::activeLevel->getPossessedPreviewCameraActor().getTag() + " (0x" + std::to_string((int)&Singleton::activeLevel->getPossessedPreviewCamera()) + ")").c_str());
#endif
		ImGui::End();
	}
	
}