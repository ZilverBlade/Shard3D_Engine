#pragma once

#include <Shard3D/core/ecs/level.h>
#include <Shard3D/core/misc/frame_info.h>
using namespace Shard3D::ECS;
namespace Shard3D {
	class LevelPeekingPanel {
	public:
		LevelPeekingPanel() = default;
		LevelPeekingPanel(const sPtr<Level>& levelContext);
		~LevelPeekingPanel();

		void setContext(const sPtr<Level>& levelContext);
		void destroyContext();

		void render(FrameInfo& frameInfo);

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