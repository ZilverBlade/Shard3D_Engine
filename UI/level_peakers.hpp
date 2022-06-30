#pragma once
#include "../wb3d/level.hpp"
#include "../wb3d/actor.hpp"
using namespace Shard3D::wb3d;
namespace Shard3D {
	class LevelPeakingPanel {
	public:
		LevelPeakingPanel() = default;
		LevelPeakingPanel(const std::shared_ptr<Level>& levelContext);
		~LevelPeakingPanel();

		void setContext(const std::shared_ptr<Level>& levelContext);
		void destroyContext();

		void render();

		Actor getSelectedActor() { return selectedActor; }
		Actor selectedActor;
	private:
		void peekTextureList();
		void peekMaterialData();
		void peekActorList();
		void peekLODman();

		std::shared_ptr<Level> context;
		
	};
}