#pragma once
#include "level_tree_panel.hpp"
#include "ImGuizmo.h"
#include "..\wb3d\level.hpp"

namespace Shard3D {
	class Gizmo {
	public:
		Gizmo();
		~Gizmo();
		void setContext(const std::shared_ptr<Level>& levelContext);
		void render(LevelTreePanel treePanel);
	private:
		std::shared_ptr<Level> level;
	};
}