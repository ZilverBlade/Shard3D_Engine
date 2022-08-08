#include "../s3dtpch.h"
#include "physics_system.hpp"
#include <Physics/Body/BodyCreationSettings.h>
#include <Physics/Character/Character.h>
#include <Physics/Collision/Shape/Shape.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include <Physics/Collision/Shape/ConvexHullShape.h>
#include <Core/JobSystem.h>
#include "../components.hpp"
namespace Shard3D {
	PhysicsSystem::PhysicsSystem() {
		
	}
	PhysicsSystem::~PhysicsSystem() {

	}
	void PhysicsSystem::simulate(FrameInfo& frameInfo, std::shared_ptr<wb3d::Level>& level) {
		//system.Update();
		level->registry.view<Components::RigidbodyComponent, Components::TransformComponent>().each([&](auto phys, auto transform) {
			
		});
	}
	//JPH::Ref<JPH::Shape> PhysicsSystem::createBoxShape(const JPH::BoxShapeSettings& settings) {
	//	JPH::Shape::ShapeResult result = settings.Create();
	//	if (result.IsValid())
	//		return result.Get();
	//	else
	//		SHARD3D_FATAL(result.GetError());
	//}
	//JPH::Ref<JPH::Shape> PhysicsSystem::createSphereShape(const JPH::SphereShapeSettings& settings) {
	//	JPH::Shape::ShapeResult result = settings.Create();
	//	if (result.IsValid())
	//		return result.Get();
	//	else
	//		SHARD3D_FATAL(result.GetError());
	//}
	//
	//JPH::Ref<JPH::Shape> PhysicsSystem::createConvexHullShape(const JPH::ConvexHullShapeSettings& settings)
	//{
	//	std::vector<JPH::Vec3> vertices = {  };
	//
	//	JPH::ConvexHullShapeSettings sets(vertices, JPH::cDefaultConvexRadius);
	//	
	//	JPH::Shape::ShapeResult result = settings.Create();
	//	if (result.IsValid())
	//		return result.Get();
	//	else
	//		SHARD3D_FATAL(result.GetError());
	//}
	//
	//void PhysicsSystem::dumpShapeToBin(JPH::Ref<JPH::Shape> shape, const std::string& path) {
	//
	//}

}
