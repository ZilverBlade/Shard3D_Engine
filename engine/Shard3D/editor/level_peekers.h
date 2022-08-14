#pragma once

#include "../core/ecs/level.h"

using namespace Shard3D::ECS;
namespace Shard3D {
	class LevelPeekingPanel {
	public:
		LevelPeekingPanel() = default;
		LevelPeekingPanel(const sPtr<Level>& levelContext);
		~LevelPeekingPanel();

		void setContext(const sPtr<Level>& levelContext);
		void destroyContext();

		void render();

	private:
		static inline bool textureInspector, materialInspector, actorInspector, lodInspector, miscInspector;

		void peekTextureInspector();
		void peekMaterialInspector();
		void peekActorInspector();
		void peekLODInspector();

		void peekMisc();

		sPtr<Level> context;
		
	};
}