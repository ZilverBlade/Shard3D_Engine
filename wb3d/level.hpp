#pragma once

#include <entt.hpp>
#include "../GUID.hpp"
#include "../utils/definitions.hpp"

namespace Shard3D {
	namespace wb3d {
		class Actor;

		class Level {
		public:
			Level();
			~Level();
			
			Actor createActor(std::string  name= "Some kind of actor");
			Actor createActorWithGUID(GUID guid, std::string name = "Some kind of actor");

			void killActor(Actor actor);
			void killEverything();

			void update(); 


			entt::registry eRegistry;
		private:
			friend class Actor;

			friend class LevelManager;

			friend class RunApp;
			//systems
			friend class BasicRenderSystem;
			friend class PointlightSystem;
			friend class SpotlightSystem;
			friend class DirectionalLightSystem;

		};
	}
}