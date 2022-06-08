#pragma once
#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"
using namespace Shard3D::wb3d;
namespace Shard3D {
	class LevelTreePanel {
	public:
		LevelTreePanel() = default;
		LevelTreePanel(const std::shared_ptr<Level>& levelContext);
		~LevelTreePanel();

		void setContext(const std::shared_ptr<Level>& levelContext);
		void destroyContext();
		void clearSelectedActor();

		void render();

		Actor getSelectedActor() { return selectedActor; }
		Actor selectedActor;
	private:
		void drawActorEntry(Actor actor);

		std::shared_ptr<Level> context;
		
	};
}