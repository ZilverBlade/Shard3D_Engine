#pragma once

#include <Jolt.h>
#include <Physics/Body/Body.h>
#include <Math/MathTypes.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include <Physics/Collision/Shape/SphereShape.h>
#include <Physics/Collision/Shape/ConvexHullShape.h>
#include <Physics/PhysicsSystem.h>

#include "../../core/misc/frame_info.h"	  
#include "../../core/ecs/level.h"
// FWD Declare
class JPH::Shape;

namespace Shard3D {
	class PhysicsSystem {
	public:
		PhysicsSystem();
		~PhysicsSystem();
		void simulate(FrameInfo& frameInfo);
		//JPH::Ref<JPH::Shape> createBoxShape(const JPH::BoxShapeSettings& settings);
		//JPH::Ref<JPH::Shape> createSphereShape(const JPH::SphereShapeSettings& settings);
		//JPH::Ref<JPH::Shape> createConvexHullShape(const JPH::ConvexHullShapeSettings& settings);
		//
		//void dumpShapeToBin(JPH::Ref<JPH::Shape> shape, const std::string& path);

	private:
		// force in m/s^2
		float gravity = 9.8f; 
		//JPH::PhysicsSystem system;
	};
}
