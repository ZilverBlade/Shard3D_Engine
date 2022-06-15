#pragma once
#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"
#include "level_tree_panel.hpp"

using namespace Shard3D::wb3d;
namespace Shard3D {
	class LevelPropertiesPanel {
	public:
		LevelPropertiesPanel() = default;
		LevelPropertiesPanel(const std::shared_ptr<Level>&levelContext);
		~LevelPropertiesPanel();

		void setContext(const std::shared_ptr<Level>&levelContext);
		void destroyContext();
		void render(LevelTreePanel tree, EngineDevice* device);
	private:
		void drawActorProperties(Actor actor, EngineDevice& device);

		std::shared_ptr<Level> context;


		const ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

	};
}
