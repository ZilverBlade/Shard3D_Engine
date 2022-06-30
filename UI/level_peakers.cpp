#include "level_peakers.hpp"
#include "../components.hpp"
#include "../wb3d/bpmgr.hpp"
#include "../utils/dialogs.h"
#include <imgui.h>
namespace Shard3D {

	LevelPeakingPanel::LevelPeakingPanel(const std::shared_ptr<Level>& levelContext) {
		setContext(levelContext);
	}

	LevelPeakingPanel::~LevelPeakingPanel() { context = nullptr; }

	void LevelPeakingPanel::setContext(const std::shared_ptr<Level>& levelContext) { context = levelContext; }
	void LevelPeakingPanel::destroyContext() { context = {}; }

	void LevelPeakingPanel::render() {
		ImGui::Begin("Level Peeking");
		

		ImGui::End();
	}
	
}