#pragma once
#include "../s3dtpch.h"
#include "../wb3d/acting_actor.hpp"

namespace Shard3D::CppScripts {
	class ExampleCppScript : public wb3d::ActingActor {
	public:
		void beginEvent() override {
			std::cout << "Example script!\n";
		}
		void endEvent()	override  {
			std::cout << "Example script ended!\n";
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