#include "../s3dtpch.h"  
#include "asset_properties_panel.hpp"
#include "../components.hpp"
#include "../wb3d/bpmgr.hpp"

#include <imgui.h>
#include "../wb3d/assetmgr.hpp"
#include "imgui_implementation.hpp"

namespace Shard3D {
	AssetPropertiesPanel::AssetPropertiesPanel(const std::string& asset) : currentAsset(asset), isOpen(1) {
		
	}

	AssetPropertiesPanel::~AssetPropertiesPanel() {}

	void AssetPropertiesPanel::render() {
		if (isOpen) {
			ImGui::Begin(std::string("Asset Properties##" + currentAsset).c_str(), &isOpen);
			if (ImGui::TreeNode(currentAsset.c_str())) {
				ImGui::TreePop();
			}
			ImGui::End();
		}
	}
}