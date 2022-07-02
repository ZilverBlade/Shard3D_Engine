#pragma once
#include "../wb3d/acting_actor.hpp"

namespace Shard3D {
	namespace CppScripts {
		class ExampleCppScript : public wb3d::ActingActor {
		public:
			void beginEvent() {}
			void endEvent()	{}

			void tickEvent(float dt) {
				// float dt = frametime
			}

			void spawnEvent() {
				std::cout << "Example script!\n";			
			}
			
			void killEvent() {
				std::cout << "Example script ended!\n";
			}
		};
	}
}