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
			
			Actor createActor(std::string  name= "Some kind of actor");
			Actor createActorWithGUID(GUID guid, std::string name = "Some kind of actor");
			//template<typename T>
			//Actor createActorWithComponent(std::string name);


			void killEverything();
			void killActor(Actor actor);

			void runGarbageCollector(VkDevice device);

			void update(); 


			entt::registry eRegistry;
			std::vector<Actor> actorQueue;

		private:
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