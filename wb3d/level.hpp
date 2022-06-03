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
			
			Actor createActor(const char* name= "Some kind of actor");
			Actor createActorWithGUID(GUID guid, const char* name = "Some kind of actor");

			void killActor(Actor actor);
			void killEverything();

			void update(); 


			entt::registry eRegistry;
		private:

			//template<typename T>
//void componentAdded();
			friend class Actor;
			friend class RunApp;

			//systems
			friend class BasicRenderSystem;
			//friend class PointlightSystem;
			//friend class SpotlightSystem;
			//friend class DirectionalLightSystem;

		};
	}
}