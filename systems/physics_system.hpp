#pragma once

#include <Jolt.h>
#include "../device.hpp"
#include <Physics/Body/Body.h>
#include <Physics/Collision/Shape/Shape.h>
#include <Math/MathTypes.h>

namespace Shard3D {
	//most bullshit operators 
	static void operator==(JPH::Vec4& rvec, const glm::vec4 & v) {
		rvec = JPH::Vec4(v.x, v.y, v.z, v.w);
		return;
	}
	static void operator==(JPH::Vec3& rvec, const glm::vec3& v) {
		rvec = JPH::Vec3(v.x, v.y, v.z);
		return;
	}

	class PhysicsSystem {
	public:
		PhysicsSystem();
		~PhysicsSystem();
		void render();
	private:
		// force in m/s^2
		float gravity = 9.8f; 
	};
}
