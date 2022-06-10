#pragma once

#include <string>
#include "../wb3d/acting_actor.hpp"
namespace Shard3D {
	namespace CppScripts {	// Components must always be in these namespaces
		class ExampleCppScript : public wb3d::ActingActor {
		public:
/**
  Gets called upon creation of the actor
 */
			void createEvent() {
				std::cout << "Example script!\n";
			}
/**
 * Gets called every frame
 * @param (float dt) Frame time of every frame
 */
			void tickEvent(float dt) {
				auto& transform = getComponent<Components::TransformComponent>();
				transform.rotation += dt;
			}
/**
 Gets called on destruction of the actor
 */
			void killEvent() {
				std::cout << "Example script ended!\n";
			}
		};
	}
}