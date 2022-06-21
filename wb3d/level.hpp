#pragma once

#include <entt.hpp>
#include "../GUID.hpp"
#include "../utils/definitions.hpp"
#include <vulkan/vulkan.h>
#include "../camera.hpp"

namespace Shard3D {
	namespace wb3d {
		class Actor;
		class Blueprint;

		enum class PlayState {
			Stopped = 0,
			Playing = 2, // unused, for future when stuff like player controls can be properly integrated in game
			Simulating = 1,
			Paused = -1
		};

		class Level {
		public:

			Level(std::string lvlName = "Some kind of level");
			~Level();

			static std::shared_ptr<Level> copy(std::shared_ptr<Level> other);

			Blueprint createBlueprint(Actor actor, std::string path, std::string name = "Some kind of blueprint");

			Actor createActor(std::string name= "Some kind of actor");
			Actor createActorWithGUID(GUID guid, std::string name = "Some kind of actor");

			void killEverything();
			void killActor(Actor actor);
			void killMesh(Actor actor);
			void reloadMesh(Actor actor);

			void runGarbageCollector(VkDevice device);

			void setPossessedCameraActor(Actor actor);
			void setPossessedCameraActor(GUID guid);
			Actor getPossessedCameraActor();
			EngineCamera& getPossessedCamera();

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

			PlayState simulationState = PlayState::Stopped;

			entt::registry registry;	
		protected:
			GUID possessedCameraActorGUID;
		private:
			std::string name = "Some kind of level";
			bool loadRegistryCapture = false;

			// queues 
			std::vector<Actor> actorKillQueue;
			std::vector<Actor> actorKillMeshQueue;
			std::vector<Actor> actorReloadMeshQueue;

			friend class Actor;
			friend class MasterManager;

			friend class LevelManager;
			friend class LevelTreePanel;
		};
	}
}