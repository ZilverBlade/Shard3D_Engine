#include "../../s3dpch.h" 

#include "physics_system.h"

#include "../../core/ecs/components.h"
namespace Shard3D {
	PhysicsSystem::PhysicsSystem() {	
		// Register allocation hook
		JPH::RegisterDefaultAllocator();

		// Install callbacks
		JPH::Trace = TraceImpl;
		
			// Create a factory
			JPH::Factory::sInstance = new JPH::Factory();

		// Register all Jolt physics types
			JPH::RegisterTypes();
			JPH::TempAllocatorImpl temp_allocator(10 * 1024 * 1024);

		// We need a job system that will execute physics jobs on multiple threads. Typically
		// you would implement the JobSystem interface yourself and let Jolt Physics run on top
		// of your own job scheduler. JobSystemThreadPool is an example implementation.
			jobsystem = make_uPtr<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

		// Create mapping table from object layer to broadphase layer
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		BPLayerInterfaceImpl broad_phase_layer_interface;

		 physics_system = make_uPtr<JPH::PhysicsSystem>();
		body_activation_listener = make_uPtr<MyBodyActivationListener>();
		contact_listener = make_uPtr<MyContactListener>();
		// Now we can create the actual physics system.
		physics_system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, MyBroadPhaseCanCollide, MyObjectCanCollide);

		// A body activation listener gets notified when bodies activate and go to sleep
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		physics_system->SetBodyActivationListener(body_activation_listener.get());

		// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		physics_system->SetContactListener(contact_listener.get());
	}
	PhysicsSystem::~PhysicsSystem() {
	
	}
	void PhysicsSystem::simulate(sPtr<ECS::Level> level, float frameTime) {
		physics_system->Update(frameTime, 0, 0, nullptr, jobsystem.get());
		level->registry.view<Components::RigidbodyComponent, Components::TransformComponent>().each([&](Components::RigidbodyComponent phys, Components::TransformComponent transform) {
			JPH::Vec3 translation = phys.physicsBody->GetPosition();
			transform.setTranslation({ translation.GetX(), translation.GetY(), translation.GetZ() });
		});
	}
	JPH::Ref<JPH::Shape> PhysicsSystem::createBoxShape(const JPH::BoxShapeSettings& settings) {
		JPH::Shape::ShapeResult result = settings.Create();
		if (result.IsValid())
			return result.Get();
		else
			SHARD3D_FATAL(result.GetError().c_str());
	}
	JPH::Ref<JPH::Shape> PhysicsSystem::createSphereShape(const JPH::SphereShapeSettings& settings) {
		JPH::Shape::ShapeResult result = settings.Create();
		if (result.IsValid())
			return result.Get();
		else
			SHARD3D_FATAL(result.GetError().c_str());
	}
	
	JPH::Ref<JPH::Shape> PhysicsSystem::createConvexHullShape(const JPH::ConvexHullShapeSettings& settings)
	{
		std::vector<JPH::Vec3> vertices = {  };
	
		JPH::ConvexHullShapeSettings sets(vertices.data(), JPH::cDefaultConvexRadius);
		
		JPH::Shape::ShapeResult result = settings.Create();
		if (result.IsValid())
			return result.Get();
		else
			SHARD3D_FATAL(result.GetError().c_str());
	}
	
	JPH::Body* PhysicsSystem::createBody(const JPH::BodyCreationSettings& settings) {
		JPH::Body* body = physics_system->GetBodyInterface().CreateBody(settings);
		physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
		return body;
	}
	
	void PhysicsSystem::dumpShapeToBin(JPH::Ref<JPH::Shape> shape, const std::string& path) {
	
	}

}
