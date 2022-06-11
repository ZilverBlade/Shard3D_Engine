#pragma once

#include <entt.hpp>
#include "../GUID.hpp"
#include "../utils/definitions.hpp"
#include <vulkan/vulkan.h>

namespace Shard3D {
	namespace wb3d {
		class Actor;

		class Level {
		public:
			Level();
			~Level();

			Actor createActor(std::string name= "Some kind of actor");
			Actor createActorWithGUID(GUID guid, std::string name = "Some kind of actor");

			void killEverything();
			void killActor(Actor actor);
			void killMesh(Actor actor);
			void reloadMesh(Actor actor);

			void runGarbageCollector(VkDevice device);

			Actor getEditorCameraActor();

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

			entt::registry registry;
			bool isPlayingLevel = false;
		private:
			bool loadRegistryCapture = false;

			void captureLevel();
			void reloadLevel();

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