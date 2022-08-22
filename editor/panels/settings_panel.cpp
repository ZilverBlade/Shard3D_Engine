#include "settings_panel.h"
#include <imgui.h>


namespace Shard3D {
	SettingsPanel::SettingsPanel() {
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

	}

	SettingsPanel::~SettingsPanel() {

	}

	void SettingsPanel::render() {
		if (showEngineSettingsWindow) renderEngineSettings();
		if (showEditorSettingsWindow) renderEditorPreferences();
		if (showGraphicsSettingsWindow) renderGraphicsSettings();
	}

	void SettingsPanel::renderEngineSettings() {
		ImGui::Begin("Engine Settings", &showEngineSettingsWindow);
		const char* items[3]{};
		items[0] = "Window";
		items[1] = "Rendering";
		items[2] = "Shader Permutation";
	
		ImGui::ListBox("##304510235325", &currentSelectedCategory, items, sizeof(items) / sizeof(items[0]));

		switch (currentSelectedCategory) {
		case (0):
			ImGui::Text("Window");
			ImGui::InputInt("Default width", &enset.DEFAULT_WIDTH, 5, 50);
			ImGui::InputInt("Default height", &enset.DEFAULT_HEIGHT, 5, 50);
			ImGui::Checkbox("Resizable", &enset.Resizable);
			ImGui::InputText("Window name", enset.WindowName, IM_ARRAYSIZE(enset.WindowName));

			break;
		case (1):
			ImGui::Text("Rendering");
			ImGui::Combo("View", &enset.ViewCombo, "Perspective\0Orthographic");

			ImGui::SliderFloat("Near clip distance", &enset.NearClipDistance, 0.000000001, 2);
			ImGui::SliderFloat("Far clip distance", &enset.FarClipDistance, 16, 32767);
			ImGui::SliderFloat("FOV", &enset.FOV, 10, 180);
			//ImGui::ColorPicker3("Background colour", enset.defaultBGColor, edpref.displayFloatOr255);
			ImGui::Checkbox("Advanced materials (PBR)", &enset.pbr);

			break;
		case (2):
			ImGui::Text("Shader Permutation");
			break;
		}


		//if (ImGui::Button("Save Changes")) {
		//    if (ini.GetSection("DEFAULT") != nullptr) {
		//        ini.SetLongValue("WINDOW", "DEFAULT_WIDTH", enset.DEFAULT_WIDTH);
		//        ini.SetLongValue("WINDOW", "DEFAULT_HEIGHT", enset.DEFAULT_HEIGHT);
		//        ini.SetBoolValue("WINDOW", "Resizable", enset.defaultBGColor[2]);
		//        ini.SetValue("WINDOW", "WindowName", enset.WindowName);
		//
		//        if (enset.ViewCombo == 0) ini.SetValue("RENDERING", "View", "Perspective");
		//        else if (enset.ViewCombo == 1) ini.SetValue("RENDERING", "View", "Orthographic");
		//        ini.SetDoubleValue("RENDERING", "NearClipDistance", enset.NearClipDistance);
		//        ini.SetDoubleValue("RENDERING", "FarClipDistance", enset.FarClipDistance);
		//        ini.SetDoubleValue("RENDERING", "FOV", enset.FOV);
		//        ini.SetDoubleValue("RENDERING", "DefaultBGColorR", enset.defaultBGColor[0]);
		//        ini.SetDoubleValue("RENDERING", "DefaultBGColorG", enset.defaultBGColor[1]);
		//        ini.SetDoubleValue("RENDERING", "DefaultBGColorB", enset.defaultBGColor[2]);
		//        ini.SetBoolValue("RENDERING", "PBR", enset.pbr);
		//
		//        ini.SaveFile(ENGINE_SETTINGS_PATH);
		//        SHARD3D_INFO("Saved engine settings succesfully");
		//    }
		//    else {
		//        SHARD3D_ERROR("Failed to write to ini file");
		//    }
		//}
		if (ImGui::Button("Revert Changes")) {}
		ImGui::End();
	
	}

	void SettingsPanel::renderEditorPreferences() {

	}


	void SettingsPanel::renderGraphicsSettings() {

		// ImGui::Begin("Graphics Settings", &showGraphicsSettingsWindow);
		//  if (ImGui::CollapsingHeader("settings", ImGuiTreeNodeFlags_None)) {
		//      if (ImGui::Checkbox("V-Sync", &GraphicsSettings::get().VSync));
		//      ImGui::DragInt("Antisotropic Filtering", &GraphicsSettings::get().maxAnisotropy, 0.08f, 1, 16);
		//  }
		//  ImGui::End();
	}

}