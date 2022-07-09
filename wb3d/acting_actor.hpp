#pragma once
#include "../s3dtpch.h"
#include "actor.hpp"
namespace Shard3D {
	namespace wb3d {
		class ActingActor {
		public:
			virtual ~ActingActor() {}

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
  Gets called upon creation of the actor
 */
			virtual void spawnEvent() {}
/**
  Gets called upon destruction of the actor
 */
			virtual void killEvent() {}
		

			void posessCameraActor(Actor actor) {
				thisActor.eLevel->setPossessedCameraActor(actor);
			}
			Level* const getActiveLevel() {
				return thisActor.eLevel;
			}
			Actor thisActor;
		private:
			friend class Level;
		};
	}
}