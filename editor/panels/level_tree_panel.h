#pragma once

#include <Shard3D/ecs.h>
using namespace Shard3D::ECS;
namespace Shard3D {
	class LevelTreePanel {
	public:
		LevelTreePanel() = default;
		LevelTreePanel(const sPtr<Level>& levelContext);
		~LevelTreePanel();

		void setContext(const sPtr<Level>& levelContext);
		void destroyContext();
		void clearSelectedActor();

		void render();

		Actor getSelectedActor() { return selectedActor; }
		Actor selectedActor;
	private:
		void drawActorEntry(Actor actor);

		sPtr<Level> context;

	};
}