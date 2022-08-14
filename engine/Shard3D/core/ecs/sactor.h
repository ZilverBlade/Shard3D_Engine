#pragma once

#include "actor.h"
namespace Shard3D {
	namespace ECS {
		class SActor {
		public:
			virtual ~SActor() {}

		protected:
/**
  Gets called upon the start of the level
 */
			virtual void beginEvent() {}
/**
 * Gets called every frame
 * @param Delta time (Time between frames)
 */
			virtual void tickEvent(float dt) {}
/**
  Gets called upon the end of the level
 */
			virtual void endEvent() {}
/**
  (unused) Gets called upon creation of the actor
 */
			virtual void spawnEvent() {}
/**
  (unused) Gets called upon destruction of the actor
 */
			virtual void killEvent() {}
		

			void possessCameraActor(Actor actor) {
				thisActor.level->setPossessedCameraActor(actor);
			}
			Level* const getActiveLevel() {
				return thisActor.level;
			}
			Actor thisActor;
		private:
			friend class Level;
		};
	}
}