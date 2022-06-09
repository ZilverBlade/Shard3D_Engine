#include "level_gizmo.hpp"

namespace Shard3D {

	Gizmo::Gizmo(){}
	Gizmo::~Gizmo(){}
	void Gizmo::setContext(const std::shared_ptr<Level>& levelContext) {
		level = levelContext;
	}
	void Gizmo::render(LevelTreePanel treePanel) {
		Actor selectedActor = treePanel.getSelectedActor();
		if (selectedActor) {
		//	ImGuizmo::SetOrthographic(false);
		//	ImGuizmo::SetDrawlist();
		//	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			auto cameraActor = level->getEditorCameraActor();
		}
	}

}