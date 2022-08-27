#pragma once
#include <Shard3D/core/ecs/sactor.h>
#include <iostream>
namespace Shard3D::CppScripts {
	class ExampleCppScript : public ECS::SActor {
	public:
		void beginEvent() override {
			std::cout << "Example script!" << "\n";
		}
		void endEvent()	override  {
			std::cout << "Example script ended!" << "\n";
		}

		void tickEvent(float dt) override {
			// float dt = frametime
		}

		void spawnEvent() override {
					
		}
		
		void killEvent() override {
		
		}
	};
}