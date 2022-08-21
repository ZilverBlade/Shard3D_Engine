#pragma once

#include <Shard3D/core/ecs/level.h>
#include <Shard3D/core/ecs/actor.h>
#include "level_tree_panel.h"
#include <imgui.h>

using namespace Shard3D::ECS;
namespace Shard3D {
	class LevelPropertiesPanel {
	public:
		LevelPropertiesPanel() = default;
		LevelPropertiesPanel(const sPtr<Level>&levelContext);
		~LevelPropertiesPanel();

		void setContext(const sPtr<Level>&levelContext);
		void destroyContext();
		void render(LevelTreePanel& tree);
	private:
		void drawActorProperties(Actor& actor);
		void drawBlueprintInfo(Actor& actor);
		void displayPreviewCamera(Actor& actor);
		sPtr<Level> context;

		bool showPreviewCamera = false;

		const ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
	};
}
