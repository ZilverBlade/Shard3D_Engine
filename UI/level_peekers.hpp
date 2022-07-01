#pragma once
#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"
using namespace Shard3D::wb3d;
namespace Shard3D {
	class LevelPeekingPanel {
	public:
		LevelPeekingPanel() = default;
		LevelPeekingPanel(const std::shared_ptr<Level>& levelContext);
		~LevelPeekingPanel();

		void setContext(const std::shared_ptr<Level>& levelContext);
		void destroyContext();

		void render();

	private:
		inline static bool textureInspector, materialInspector, actorInspector, lodInspector;

		void peekTextureInspector();
		void peekMaterialInspector();
		void peekActorInspector();
		void peekLODInspector();

		std::shared_ptr<Level> context;
		
	};
}