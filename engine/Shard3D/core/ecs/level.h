#pragma once
#include <entt.hpp>
#include "../misc/UUID.h"
#include "../../s3dstd.h"
#include "../../core.h"
#include "../vulkan_api/device.h"
#include "../rendering/camera.h"
namespace Shard3D {
	class PhysicsSystem;
	class Application;
	namespace ECS {
		class Actor;
		class LevelTreePanel;
		enum class PlayState {
			Stopped = 0, // no scripts have been instantiated. outside of the editor this should only be this state when loading levels.
			Simulating = 2, // unused, for future when stuff physics can be previewed.
			Playing = 1, // indicates that the level is being played in the editor.
			Paused = -1, // regular pause mode, for the editor, where all input pauses including HUD, and repossesses editor camera.
			PausedRuntime = -2	// special pause state where everything pauses except UI input, for game runtime like pause menu etc.
		};

		class Level {
		public:
			DELETE_COPY(Level);

			Level(const std::string& lvlName = "Some kind of level", bool makeSystem = true);
			~Level();

			static sPtr<Level> copy(sPtr<Level> other);
			static void shallowCopy(sPtr<Level>& this_, sPtr<Level>& other);

			Actor createActor(const std::string& name= "Some kind of actor");
			
			void killEverything();
			void killActor(Actor actor);
			
			void runGarbageCollector(EngineDevice& device);

			Actor getActorFromUUID(UUID guid);
			// Unreliable function as multiple actors can have identical tags, use getActorFromUUID() whenever possible.
			Actor getActorFromTag(const std::string& tag);

			void setPossessedCameraActor(Actor actor);
			void setPossessedCameraActor(UUID guid);
			Actor getPossessedCameraActor();
			EngineCamera& getPossessedCamera();

			void parentActor(Actor child, Actor parent);

			/* *
* Call when level events must begin
*/
			void begin();
			/* *
* Gets called upon every frame
* \param dt Frame time
*/
			void tick(float dt);
			/* *
* Call when level events must end
*/
			void end();

			void setPhysicsSystem(PhysicsSystem* _physicsSystemPtr) { physicsSystemPtr = _physicsSystemPtr; }
			PhysicsSystem* physicsSystemPtr;

			void simulationStateCallback();

			PlayState simulationState = PlayState::Stopped;

			bool wasPaused;

			entt::registry registry;
			std::string name = "Some kind of level";
			std::string currentpath;
		protected:
			UUID possessedCameraActorGUID;
			UUID _possessedCameraActorGUID;
		private:
			//should only be called by system processes
			Actor createActorWithUUID(UUID guid, const std::string& name = "Some kind of actor");
			
			void rebuildTransforms();
			void rebuildRelations(Actor child);

			// map
			hashMap<UUID, entt::entity> actorMap;

			// queues 
			std::vector<Actor> actorKillQueue;

			friend class Shard3D::Application;
			friend class Actor;
			friend class MasterManager;

			friend class LevelManager;
			friend class LevelTreePanel;
			friend class LevelPropertiesPanel;
			friend class LevelPeekingPanel;
		};
	}
}