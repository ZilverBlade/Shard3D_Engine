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

			Actor createChild(Actor actor, std::string name = "Some kind of actor");
			Actor createChildWithGUID(GUID guid, Actor actor, std::string name);

			//template<typename T>
			//Actor createActorWithComponent(std::string name);

			void killEverything();
			void killActor(Actor actor);
			void killChild(Actor parentActor, Actor childActor);
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

			entt::registry eRegistry;

		private:
			// queues 
			std::vector<Actor> actorKillQueue;
			std::vector<Actor> actorKillMeshQueue;
			std::vector<Actor> actorReloadMeshQueue;

			friend class Actor;

			friend class LevelManager;
			friend class LevelTreePanel;

			friend class RunApp;
			//systems
			friend class BasicRenderSystem;
			friend class PointlightSystem;
			friend class SpotlightSystem;
			friend class DirectionalLightSystem;

		};
	}
}