#include "../../s3dpch.h" 

#include "physics_system.h"
#include <Physics/Body/BodyCreationSettings.h>
#include <Physics/Character/Character.h>
#include <Physics/Collision/Shape/Shape.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include <Physics/Collision/Shape/ConvexHullShape.h>
#include <Core/JobSystem.h>
#include "../../core/ecs/components.h"
namespace Shard3D {
	PhysicsSystem::PhysicsSystem() {
		
	}
	PhysicsSystem::~PhysicsSystem() {

	}
	void PhysicsSystem::simulate(FrameInfo& frameInfo) {
		//system.Update();
		frameInfo.activeLevel->registry.view<Components::RigidbodyComponent, Components::TransformComponent>().each([&](auto phys, auto transform) {
			
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
