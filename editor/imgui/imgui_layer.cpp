#include "imgui_layer.h"
#include "imgui_implementation.h"

#include <imgui.h>

#include <imgui_internal.h>

#include "imgui_glfw_implementation.h"

#include <miniaudio.h>

#include <Shard3D/ecs.h>
#include <Shard3D/core/misc/graphics_settings.h>
#include <Shard3D/core/asset/assetmgr.h>
#include <Shard3D/core/ui/hud_layer.h>
#include <Shard3D/core/audio/audio.h>
#include <Shard3D/workarounds.h>
#include <Shard3D/utils/dialogs.h>

#include "imgui_initializer.h"
#include <shellapi.h>
#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>
#include <Shard3D/systems/buffers/material_system.h>
#include <Shard3D/core/ecs/levelmgr.h>


namespace Shard3D {
	ImGuiLayer::ImGuiLayer(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass) : engineDevice(dvc), engineWindow(wnd){
		std::string title = "Shard3D Engine " + ENGINE_VERSION.toString() + " (Playstate: Stopped)";
		glfwSetWindowTitle(engineWindow.getGLFWwindow(), title.c_str());

		refreshContext = true;

		// set the default values for the structure from ini file so that you can actually modify them
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		enset.DEFAULT_WIDTH = ini.GetLongValue("WINDOW", "DEFAULT_WIDTH");
		enset.DEFAULT_HEIGHT = ini.GetLongValue("WINDOW", "DEFAULT_HEIGHT");
		enset.Resizable = ini.GetBoolValue("WINDOW", "Resizable");
		strncpy(enset.WindowName, ini.GetValue("WINDOW", "WindowName"), 64);

		if ((std::string)ini.GetValue("RENDERING", "View") == "Perspective") enset.ViewCombo = 0;
		else if ((std::string)ini.GetValue("RENDERING", "View") == "Orthographic") enset.ViewCombo = 1;
		enset.NearClipDistance = ini.GetDoubleValue("RENDERING", "NearClipDistance");
		enset.FarClipDistance = ini.GetDoubleValue("RENDERING", "FarClipDistance");
		enset.FOV = ini.GetDoubleValue("RENDERING", "FOV");
		enset.defaultBGColor[0] = ini.GetDoubleValue("RENDERING", "DefaultBGColorR");
		enset.defaultBGColor[1] = ini.GetDoubleValue("RENDERING", "DefaultBGColorG");
		enset.defaultBGColor[2] = ini.GetDoubleValue("RENDERING", "DefaultBGColorB");
		enset.pbr = ini.GetBoolValue("RENDERING", "PBR");

		createIcons();
	}

	ImGuiLayer::~ImGuiLayer() {}

	static void loadLevel(sPtr<ECS::Level>& level, const std::string& path) {
		if (level->simulationState != PlayState::Stopped) level->end();
		sPtr<ECS::Level> newlevel = make_sPtr<ECS::Level>();
		Level::shallowCopy(newlevel,level);
		LevelManager levelMan(newlevel);
		LevelMgrResults result = levelMan.load(path, false);
		if (result == LevelMgrResults::OldEngineVersionResult) {
			if (MessageDialogs::show("This level was created in an different version of Shard3D, results may be unexpected.\nWould you like to try and load the level anyway?", "WARNING!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
				levelMan.load(path, true);
			}
			else {
				levelMan.load(level->currentpath, true);
			}
		}
		level.swap(newlevel);
	}
	static void saveLevel(sPtr<ECS::Level>& level, const std::string& path) {
		if (level->simulationState != PlayState::Stopped) { SHARD3D_ERROR("Cannot save level that is running!"); return; }
		ECS::LevelManager levelMan(level);
		levelMan.save(path, false);
	}

	static void playLevel(sPtr<ECS::Level>& level, sPtr<ECS::Level>& capturedLevel, GLFWwindow* window) {
		SHARD3D_INFO("Capturing level");
		capturedLevel = Level::copy(level);
		level->begin();

		std::string title = "Shard3D Engine " + ENGINE_VERSION.toString() + " (Playstate: PLAYING) | " + level->name;
		glfwSetWindowTitle(window, title.c_str());
	}
	static void pauseLevel(sPtr<ECS::Level>& level, GLFWwindow* window) {
		level->simulationState = PlayState::Paused;
		level->simulationStateCallback();
		std::string title = "Shard3D Engine " + ENGINE_VERSION.toString() + " (Playstate: Paused) | " + level->name;
		glfwSetWindowTitle(window, title.c_str());
	}
	static void resumeLevel(sPtr<ECS::Level>& level, GLFWwindow* window) {
		level->simulationState = PlayState::Playing;
		level->simulationStateCallback();
		std::string title = "Shard3D Engine " + ENGINE_VERSION.toString() + " (Playstate: PLAYING) | " + level->name;
		glfwSetWindowTitle(window, title.c_str());
	}
	static void stopLevel(sPtr<ECS::Level>& level, sPtr<ECS::Level>& capturedLevel, GLFWwindow* window) {	
		level->end();
		SHARD3D_INFO("Loading back Captured level");
		level.swap(capturedLevel);
		capturedLevel = make_sPtr<Level>("__WB3D:NOEDITcapturelvl");

		std::string title = "Shard3D Engine " + ENGINE_VERSION.toString() + " (Playstate: Stopped) | " + level->name;
		glfwSetWindowTitle(window, title.c_str());
	}


	void ImGuiLayer::createIcons() {
		{
			auto& img = _special_assets::_editor_icons.at("editor.play");
			icons.play = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.pause");
			icons.pause = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.stop");
			icons.stop = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.pref");
			icons.pref = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);       
		} {
			auto& img = _special_assets::_editor_icons.at("editor.save");
			icons.save = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.preview");
			icons.preview = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);       
		} {
			auto& img = _special_assets::_editor_icons.at("editor.layout");
			icons.layout = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.load");
			icons.load = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.level");
			icons.level = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.viewport");
			icons.viewport = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.settings");
			icons.settings = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		} {
			auto& img = _special_assets::_editor_icons.at("editor.launch");
			icons.launchgame = ImGui_ImplVulkan_AddTexture(img->getSampler(), img->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	
	}

	void ImGuiLayer::detach() {
		vkDeviceWaitIdle(engineDevice.device());
		vkDestroyDescriptorPool(engineDevice.device(), ImGuiInitializer::imGuiDescriptorPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		levelTreePanel.destroyContext();
		levelPropertiesPanel.destroyContext();
		levelPeekPanel.destroyContext();

		materialBuilder.destroy();
	}

	void ImGuiLayer::render(FrameInfo& frameInfo) {
		if (refreshContext) {
			levelTreePanel.setContext(frameInfo.activeLevel);
			levelPropertiesPanel.setContext(frameInfo.activeLevel);
			levelPeekPanel.setContext(frameInfo.activeLevel);
			refreshContext = false;
		}
		
		SHARD3D_EVENT_BIND_HANDLER_RFC(engineWindow, ImGuiLayer::eventEvent);

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = frameInfo.frameTime;

		glfwGetWindowSize(engineWindow.getGLFWwindow(), &width, &height);
		io.DisplaySize = ImVec2(width, height);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ax::NodeEditor::SetCurrentEditor(nodeEditorContext);
		static bool visible = true;

		//ImGui::ShowDemoWindow(&visible);
		
#pragma region boilerplate dockspace code    
		static bool opt_fullscreen = true;
		static bool opt_padding = true;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.f));
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else { dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode; }
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("MyDockSpace", &visible, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();
		if (opt_fullscreen)
			ImGui::PopStyleVar(2);
#pragma endregion
		// Submit the DockSpace
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			renderMenuBar(frameInfo);
			ImGui::EndMenuBar();
		}
		ImGui::PopStyleColor();
		renderQuickBar(frameInfo);

		renderViewport(frameInfo);

		// start rendering stuff here
		//ImGuizmo::BeginFrame();
		hudBuilder.render();
		componentActorPanels.render();
		materialBuilder.render();
		levelTreePanel.render();
		levelPropertiesPanel.render(levelTreePanel);
		levelPeekPanel.render(frameInfo);
		assetExplorerPanel.render();
		settingsPanel.render();
		//levelGizmo.render(level, levelTreePanel);


		if (showCredits) {
			ImGui::Begin("Credits", &showCredits);
			if (ImGui::TreeNodeEx("Programming", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("ZilverBlade (Lead)");
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Graphics", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("ZilverBlade (Lead)");
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Editor Design", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("ZilverBlade (Layout and Colours)");
				ImGui::Text("Dhibaid (Icons)");
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Special thanks", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Brendan Galea (Vulkan tutorial series)");
				ImGui::Text("The Cherno (Game engine series)");
				ImGui::Text("All of the creators of the used libraries");
				ImGui::TreePop();
			}
			ImGui::End();
		}

		if (showStatsWindow) {
			ImGui::Begin("Stats");
			timeSinceLastSecond += frameInfo.frameTime;
			if (timeSinceLastSecond > 1.f) { deltaTimeFromLastSecond = frameInfo.frameTime; timeSinceLastSecond = 0; }
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTimeFromLastSecond * 1000.f, 1.f / deltaTimeFromLastSecond);
			ImGui::End();
		}
		if (showStylizersWindow) {
			ImGui::Begin("Stylizer");
			ImGui::ColorEdit4("Ambient colour", glm::value_ptr(GraphicsSettings::getRuntimeInfo().ambientLightColor));
			ImGui::ShowStyleEditor();
			ImGui::End();
		}
		if (showDemoWindow) ImGui::ShowDemoWindow();
		ImGui::End();

		SHARD3D_STAT_RECORD();

		ImGui::Render();
		SHARD3D_STAT_RECORD_END({ "ImGui", "Render" });

		SHARD3D_STAT_RECORD();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frameInfo.commandBuffer);
		SHARD3D_STAT_RECORD_END({ "ImGui", "ImGui_ImplVulkan_RenderDrawData" });
		SHARD3D_STAT_RECORD();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SHARD3D_STAT_RECORD_END({ "ImGui", "UpdatePlatformWindowsRenderPlatformWindowsDefault" });

}

	void ImGuiLayer::attachGUIEditorInfo(sPtr<HUDContainer>& container) {
		hudBuilder.setContext(container);
	}

	void ImGuiLayer::renderViewport(FrameInfo& frameInfo) {
		ImGui::Begin("Viewport");

		ImVec2 vSize = ImGui::GetContentRegionAvail();
		GraphicsSettings::getRuntimeInfo().aspectRatio = vSize.x / vSize.y;
		GraphicsSettings::getRuntimeInfo().localScreen = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x ,ImGui::GetWindowSize().y};

		ImGui::Image(viewportImage, vSize);

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.ASSEXP.LVL")) {
				if (MessageDialogs::show("This will overwrite the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MessageDialogs::OPTYESNO | MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTDEFBUTTON2) == MessageDialogs::RESYES) {
					levelTreePanel.clearSelectedActor();
					loadLevel(frameInfo.activeLevel, std::string(ENGINE_ASSETS_PATH + std::string("\\") + (char*)payload->Data));
					refreshContext = true;
				}
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHARD3D.COMPONENTS.DROP")) {
				size_t h_code = *reinterpret_cast<size_t*>(payload->Data);
				if (h_code == typeid(Components::PointlightComponent).hash_code())
					frameInfo.activeLevel->createActor("Pointlight").addComponent<Components::PointlightComponent>();
				else if (h_code == typeid(Components::SpotlightComponent).hash_code())
					frameInfo.activeLevel->createActor("Spotlight").addComponent<Components::SpotlightComponent>();
				else if (h_code == 32325235)
					frameInfo.activeLevel->createActor("Cube").addComponent<Components::Mesh3DComponent>(ResourceHandler::coreAssets.m_defaultModel);
				else if (h_code == typeid(Components::CameraComponent).hash_code()) {
					Actor actor = frameInfo.activeLevel->createActor("Camera Actor"); actor.addComponent<Components::Mesh3DComponent>(AssetID("assets/_engine/msh/camcord.obj" ENGINE_ASSET_SUFFIX));
					actor.addComponent<Components::CameraComponent>();
					actor.getComponent<Components::CameraComponent>().postProcessMaterials.emplace_back(ResourceHandler::retrievePPOMaterial(AssetID("assets/_engine/mat/ppo/hdr_vfx.s3dasset")).get(), AssetID("assets/_engine/mat/ppo/hdr_vfx.s3dasset"));
					actor.getComponent<Components::CameraComponent>().postProcessMaterials.emplace_back(ResourceHandler::retrievePPOMaterial(AssetID("assets/_engine/mat/ppo/bloom_vfx.s3dasset")).get(), AssetID("assets/_engine/mat/ppo/bloom_vfx.s3dasset"));
				}
			}
		}
		
		if (frameInfo.activeLevel->simulationState != PlayState::Playing) {	
			if (ImGui::IsWindowHovered()) {
				Actor cameraActor = frameInfo.activeLevel->getActorFromUUID(0);
				editorCameraController.tryPollOrientation(engineWindow, frameInfo.frameTime, cameraActor);
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) ImGui::FocusWindow(ImGui::GetCurrentWindow());
			}
			if (ImGui::IsWindowFocused()) {
				Actor cameraActor = frameInfo.activeLevel->getActorFromUUID(0);
				editorCameraController.tryPollTranslation(engineWindow, frameInfo.frameTime, cameraActor);
			}
		}

		{
			Actor actor = levelTreePanel.getSelectedActor();
			if (actor) {
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
				Actor cameraActor = frameInfo.activeLevel->getPossessedCameraActor();
				EngineCamera& camera = cameraActor.getComponent<Components::CameraComponent>().camera;
				glm::mat4 proj = camera.getProjection();
				glm::mat4 invView = camera.getView();

				auto& tc = actor.getComponent<Components::TransformComponent>();
				glm::mat4 actorTransform = tc.transformMatrix;

				gizmoType = ImGuizmo::TRANSLATE;
				ImGuizmo::Manipulate(glm::value_ptr(invView), glm::value_ptr(proj), (ImGuizmo::OPERATION)gizmoType, ImGuizmo::LOCAL, glm::value_ptr(actorTransform));
				if (ImGuizmo::IsUsing() && gizmoType != -1) {
					glm::vec3 transl{};
					glm::vec3 rotat{};
					glm::vec3 scal{};

					Components::TransformComponent::decompose(actorTransform, &transl, &rotat, &scal);
					glm::vec4 realTranslation = glm::vec4(transl.x, transl.z, transl.y, 1.f);
					glm::vec4 realRotation = glm::vec4(rotat.x, rotat.z, rotat.y, 1.f);
					glm::vec4 realScale = glm::vec4(scal.x, scal.z, scal.y, 1.f);

					glm::vec3 original_rotation = tc.getRotation();

					if (!actor.hasRelationship()) {
						glm::vec3 deltaRotation = glm::vec3(realRotation) - original_rotation; 
						tc.setTranslation(glm::vec3(realTranslation));
						tc.setScale(glm::vec3(realScale));
						//tc.setRotation(deltaRotation + original_rotation);
					}
					else if (actor.getComponent<Components::RelationshipComponent>().parentActor == entt::null) {
						glm::vec3 deltaRotation = glm::vec3(realRotation) - original_rotation;
						tc.setTranslation(glm::vec3(realTranslation));
						tc.setScale(glm::vec3(realScale));
						tc.setRotation(deltaRotation + original_rotation);
					}
					else {
						auto& parentTransform = Actor(actor.getComponent<Components::RelationshipComponent>().parentActor, frameInfo.activeLevel.get()).getTransform().transformMatrix;
						glm::vec4 adjustedTransform = actorTransform[3] / parentTransform;
					}
				}
			}
		}
		if (frameInfo.activeLevel->simulationState == PlayState::Paused) {
			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			vMin.x += ImGui::GetWindowPos().x + 8;
			vMin.y += ImGui::GetWindowPos().y + 16;
			ImGui::GetWindowDrawList()->AddImage(icons.pause, vMin, ImVec2(vMin.x + 64, vMin.y + 64), ImVec2(0, 0), ImVec2(1, 1));
		}
		if (GraphicsSettings::editorPreview.ONLY_GAME) {
			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			vMin.x += ImGui::GetWindowPos().x + 80;
			vMin.y += ImGui::GetWindowPos().y + 16;
			// ImGui::GetWindowDrawList()->AddText(io.Fonts->Fonts[0], 14.f, ImVec2(vMin.x, vMin.y), ImU32(1.f), "Test");
			ImGui::GetWindowDrawList()->AddImage(icons.preview, vMin, ImVec2(vMin.x + 64, vMin.y + 64), ImVec2(0, 0), ImVec2(1, 1));
		}

		ImGui::End();
	}

	void ImGuiLayer::renderQuickBar(FrameInfo& frameInfo) {

		ImGui::Begin("_editor_quickbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
		ImVec2 btnSize = { 48.f, 48.f };
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = std::max((int)(panelWidth / (btnSize.x + 16.f))// <--- thumbnail size (96px) + padding (16px)
			, 1);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
		ImGui::Columns(columnCount, 0, false);                
		// begin

		// Save/Load
		if (ImGui::ImageButton(icons.save, btnSize)) {
			saveLevel(frameInfo.activeLevel, frameInfo.activeLevel->currentpath);
		}
		ImGui::TextWrapped("Save");
		ImGui::NextColumn();
		if (ImGui::ImageButton(icons.load, btnSize)) {
			if (MessageDialogs::show("This will overwrite the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MessageDialogs::OPTYESNO | MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTDEFBUTTON2) == MessageDialogs::RESYES) {
				std::string filepath = FileDialogs::openFile(ENGINE_SHARD3D_LEVELFILE_OPTIONS);
				if (!filepath.empty()) {
					levelTreePanel.clearSelectedActor();
					loadLevel(frameInfo.activeLevel, filepath);
					refreshContext = true;
				}
			}
		}
		ImGui::TextWrapped("Load");
		ImGui::NextColumn();
		ImGui::NextColumn();

		// PLAY MODE
		if (ImGui::ImageButton(
			(frameInfo.activeLevel->simulationState == PlayState::Stopped) ? icons.play : 
				((frameInfo.activeLevel->simulationState == PlayState::Paused) ? icons.play : icons.pause), btnSize))
			if (frameInfo.activeLevel->simulationState == PlayState::Stopped) { // Play
				playLevel(frameInfo.activeLevel, capturedLevel, engineWindow.getGLFWwindow());
			} else if (frameInfo.activeLevel->simulationState == PlayState::Paused) {  // Resume
				resumeLevel(frameInfo.activeLevel, engineWindow.getGLFWwindow());
			} else if (frameInfo.activeLevel->simulationState == PlayState::Playing) {  // Pause
				pauseLevel(frameInfo.activeLevel, engineWindow.getGLFWwindow());
			}
		ImGui::TextWrapped((frameInfo.activeLevel->simulationState == PlayState::Stopped) ? "Play" :
			((frameInfo.activeLevel->simulationState == PlayState::Paused) ? "Resume" : "Pause"));
		ImGui::NextColumn();
		ImGui::BeginDisabled(frameInfo.activeLevel->simulationState == PlayState::Stopped);
		if (ImGui::ImageButton(icons.stop, btnSize)) {        // Stop
			levelTreePanel.clearSelectedActor();
			stopLevel(frameInfo.activeLevel, capturedLevel, engineWindow.getGLFWwindow());
			refreshContext = true;
		}
		ImGui::TextWrapped("Stop");
		ImGui::EndDisabled();

		ImGui::NextColumn();
		ImGui::NextColumn();
		// Editor settings
		if (ImGui::ImageButton(icons.pref, btnSize)) {
			SHARD3D_NOIMPL;
		}
		ImGui::TextWrapped("Pref");
		ImGui::NextColumn();
		// Editor settings
		if (ImGui::ImageButton(icons.settings, btnSize)) {
			SHARD3D_NOIMPL;
		}
		ImGui::TextWrapped("Config");
		ImGui::NextColumn();
		if (ImGui::ImageButton(icons.layout, btnSize)) {
			SHARD3D_NOIMPL;
		}
		ImGui::TextWrapped("Layout");
		ImGui::NextColumn();
		if (ImGui::ImageButton(icons.launchgame, btnSize)) {
			ShellExecuteA(nullptr, "open", "Shard3DRuntime.exe", frameInfo.activeLevel->currentpath.c_str(), "", true);
		}
		ImGui::TextWrapped("Launch");
		ImGui::NextColumn();
		if (ImGui::ImageButton(icons.viewport, btnSize)) {
			SHARD3D_NOIMPL;
		}
		ImGui::TextWrapped("View");
		ImGui::NextColumn();
		
		// end
		ImGui::Columns(1);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	void ImGuiLayer::renderMenuBar(FrameInfo& frameInfo) {
		if (ImGui::BeginMenu("File")) {
			ImGui::TextDisabled("SHARD3D 0.1");
			ImGui::Separator();
			ImGui::BeginDisabled(frameInfo.activeLevel->simulationState != PlayState::Stopped);
			if (ImGui::MenuItem("New Level", "Ctrl+N")) {
				if (MessageDialogs::show("This will destroy the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MessageDialogs::OPTYESNO | MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTDEFBUTTON2) == MessageDialogs::RESYES) {
					levelTreePanel.clearSelectedActor();
					frameInfo.activeLevel->killEverything();
				}
			}
			if (ImGui::MenuItem("Load Level...", "Ctrl+O")) {
				if (MessageDialogs::show("This will overwrite the current level, and unsaved changes will be lost! Are you sure you want to continue?", "WARNING!", MessageDialogs::OPTYESNO | MessageDialogs::OPTICONEXCLAMATION | MessageDialogs::OPTDEFBUTTON2) == MessageDialogs::RESYES) {
					std::string filepath = FileDialogs::openFile(ENGINE_SHARD3D_LEVELFILE_OPTIONS);
					if (!filepath.empty()) {
						levelTreePanel.clearSelectedActor();
						loadLevel(frameInfo.activeLevel, filepath);
						refreshContext = true;	
					}
				}
			}
			if (ImGui::MenuItem("Save Level...", "Ctrl+S")) {
				saveLevel(frameInfo.activeLevel, frameInfo.activeLevel->currentpath);
			}
			if (ImGui::MenuItem("Save Level As...", "Ctrl+Shift+S")) {
				std::string filepath = FileDialogs::saveFile(ENGINE_SHARD3D_LEVELFILE_OPTIONS);
				if (!filepath.empty()) {
					saveLevel(frameInfo.activeLevel, filepath);
				}
			}
			ImGui::Separator();
			ImGui::EndDisabled();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			ImGui::TextDisabled("SHARD3D 0.1");
			ImGui::Separator();
			if (ImGui::BeginMenu("Level Simulation")) {
				ImGui::BeginDisabled(frameInfo.activeLevel->simulationState != PlayState::Stopped || frameInfo.activeLevel->simulationState == PlayState::Paused);
				if (ImGui::MenuItem("Begin")) {
					playLevel(frameInfo.activeLevel, capturedLevel, engineWindow.getGLFWwindow());		
				} ImGui::EndDisabled();
				if (frameInfo.activeLevel->simulationState != PlayState::Paused) {
					ImGui::BeginDisabled(frameInfo.activeLevel->simulationState != PlayState::Playing); if (ImGui::MenuItem("Pause")) {
						pauseLevel(frameInfo.activeLevel, engineWindow.getGLFWwindow());
					} ImGui::EndDisabled();
				}
				else {
					ImGui::BeginDisabled(frameInfo.activeLevel->simulationState == PlayState::Playing); if (ImGui::MenuItem("Resume")) {
						resumeLevel(frameInfo.activeLevel, engineWindow.getGLFWwindow());
					} ImGui::EndDisabled();
				}

				ImGui::BeginDisabled(frameInfo.activeLevel->simulationState == PlayState::Stopped);
				if (ImGui::MenuItem("End")) {
					levelTreePanel.clearSelectedActor();
					stopLevel(frameInfo.activeLevel, capturedLevel, engineWindow.getGLFWwindow());
					refreshContext = true;
				} ImGui::EndDisabled();
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create Blueprint")) {}
			if (ImGui::MenuItem("Create Module Definition")) {}
			//if (ImGui::MenuItem("", NULL /*make sure to add some sort of shardcut */)) {}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
#ifndef _DEPLOY
			if (ImGui::BeginMenu("Rendering")) {
				ImGui::Checkbox("Preview Game", &GraphicsSettings::editorPreview.ONLY_GAME);
				ImGui::Separator();
				ImGui::Checkbox("Grid", &GraphicsSettings::editorPreview.V_GRID);
				ImGui::Checkbox("Billboards", &GraphicsSettings::editorPreview.V_EDITOR_BILLBOARDS);
				ImGui::Checkbox("HUD", &GraphicsSettings::editorPreview.V_GUI);
				ImGui::EndMenu();
			}
#endif
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug")) {
			ImGui::TextDisabled("Shard3D Debug menu");
			ImGui::Checkbox("Stylizer", &showStylizersWindow);
			ImGui::Checkbox("ImGuiDemo", &showDemoWindow);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Actions")) {
			if (ImGui::MenuItem("Dump current frame's stats", NULL /*make sure to add some sort of shardcut */)) SHARD3D_STAT_DUMP_ALL();
			if (ImGui::MenuItem("Read graphics settings", NULL /*make sure to add some sort of shardcut */)) GraphicsSettings::read();
			if (ImGui::MenuItem("Recompile Surface Materials", NULL /*make sure to add some sort of shardcut */)) {
				MaterialSystem::recompileSurface();
			}
			if (ImGui::MenuItem("Recompile Post Processing Materials", NULL /*make sure to add some sort of shardcut */)) {
				MaterialSystem::recompilePPO();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.

			ImGui::TextDisabled("SHARD3D 0.1");
			ImGui::Separator();
			if (ImGui::MenuItem("Engine Settings", NULL /*make sure to add some sort of shardcut */)) { showEngineSettingsWindow = true; }
			if (ImGui::MenuItem("SHARD3D Settings", NULL /*make sure to add some sort of shardcut */)) { /*show editor win*/ }
			ImGui::Separator();
			if (ImGui::MenuItem("Game Graphics Settings", NULL /*make sure to add some sort of shardcut */)) { showGraphicsSettingsWindow = true; }
			ImGui::Separator();
			if (ImGui::MenuItem("Material Builder", NULL /*make sure to add some sort of shardcut */)) {
				showTest = true;
			}
			ImGui::Separator();
			ImGui::Checkbox("Stats", &showStatsWindow);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
#ifdef WIN32
			if (ImGui::MenuItem("Main Website")) { ShellExecuteA(nullptr, "open", "https://www.shard3d.com", nullptr, nullptr, false); }
			if (ImGui::MenuItem("Documentation")) { ShellExecuteA(nullptr, "open", "https://docs.shard3d.com", nullptr, nullptr, false); }
			if (ImGui::MenuItem("SHARD3D")) { ShellExecuteA(nullptr, "open", "https://docs.shard3d.com/SHARD3D.html", nullptr, nullptr, false); }
#endif  
#ifdef __linux__ 
			ImGui::MenuItem("Unsupported");
#endif
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Credits")) showCredits = true;
	}


	void ImGuiLayer::eventEvent(Events::Event& e) {
		SHARD3D_EVENT_CREATE_DISPATCHER(e);
		dispatcher.dispatch<Events::MouseButtonDownEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseButtonDownEvent));
		dispatcher.dispatch<Events::MouseButtonReleaseEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseButtonReleaseEvent));
		dispatcher.dispatch<Events::MouseHoverEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseHoverEvent));
		dispatcher.dispatch<Events::MouseScrollEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::mouseScrollEvent));
		dispatcher.dispatch<Events::KeyDownEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::keyDownEvent));
		dispatcher.dispatch<Events::KeyReleaseEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::keyReleaseEvent));
		dispatcher.dispatch<Events::KeyPressEvent>(SHARD3D_EVENT_BIND_VOID(ImGuiLayer::keyPressEvent));
	}

	bool ImGuiLayer::mouseButtonDownEvent(Events::MouseButtonDownEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getButtonCode()] = true;

		return false;
	}

	bool ImGuiLayer::mouseButtonReleaseEvent(Events::MouseButtonReleaseEvent& e)  {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getButtonCode()] = false;

		return false;
	}

	bool ImGuiLayer::mouseHoverEvent(Events::MouseHoverEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.getXPos(), e.getYPos());

		return false;
	}

	bool ImGuiLayer::mouseScrollEvent(Events::MouseScrollEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.getYOffset();
		io.MouseWheelH += e.getXOffset();

		return false;
	}
	
	bool ImGuiLayer::keyDownEvent(Events::KeyDownEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = true;
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		return false;
	}

	bool ImGuiLayer::keyReleaseEvent(Events::KeyReleaseEvent& e)  {
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = false;
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		return false;
	}

	bool ImGuiLayer::keyPressEvent(Events::KeyPressEvent& e)  {
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter((unsigned short)e.getKeyCode());
		return false;
	}




}