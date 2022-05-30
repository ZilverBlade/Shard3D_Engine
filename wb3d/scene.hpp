#pragma once

#include <entt.hpp>
#include "../game_object.hpp"


namespace Shard3D {
	namespace wb3d {
		class Actor;

		class Scene {
		public:
				Scene();
				~Scene();
				
				Actor createActor(std::string name = "Some kind of actor");
				void killActor(Actor actor);

				void update(); 
				

		private:
			//template<typename T>
			//void componentAdded();
			entt::registry eRegistry;
			
			friend class Actor;

			//systems
			friend class BasicRenderSystem;
			friend class PointlightSystem;
			friend class SpotlightSystem;
			friend class DirectionalLightSystem;

		};
	}
}