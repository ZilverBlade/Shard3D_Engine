#include "../../s3dpch.h" 

#include "physics_system.h"

#include "../../core/ecs/components.h"
#include "../../core/ecs/level.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
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
			 temp_allocator = make_uPtr<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

		// We need a job system that will execute physics jobs on multiple threads. Typically
		// you would implement the JobSystem interface yourself and let Jolt Physics run on top
		// of your own job scheduler. JobSystemThreadPool is an example implementation.
			jobsystem = make_uPtr<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() / 2);

		// Create mapping table from object layer to broadphase layer
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		
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



		// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	}
	PhysicsSystem::~PhysicsSystem() {
	
	}
	void PhysicsSystem::begin(ECS::Level* level) {
		level->registry.view<Components::RigidbodyComponent, Components::TransformComponent>().each([&](Components::RigidbodyComponent& phys, Components::TransformComponent& transform) {
			physics_system->GetBodyInterface().ActivateBody(phys.physicsBody);
			glm::mat4 transformMat = transform.transformMatrix;
			JPH::Mat44 matr = JPH::Mat44();
			matr.SetColumn4(0, { transformMat[0][0], transformMat[0][1], transformMat[0][2], transformMat[0][3], });
			matr.SetColumn4(1, { transformMat[1][0], transformMat[1][1], transformMat[1][2], transformMat[1][3], });
			matr.SetColumn4(2, { transformMat[2][0], transformMat[2][1], transformMat[2][2], transformMat[2][3], });
			matr.SetColumn4(3, { transformMat[3][0], transformMat[3][1], transformMat[3][2], transformMat[3][3], });
			physics_system->GetBodyInterface().SetPosition(phys.physicsBody, { transform.getTranslation().x, transform.getTranslation().z, transform.getTranslation().y }, JPH::EActivation::Activate);

			//physics_system->GetBodyInterface().SetAngularVelocity(phys.physicsBody, { 0.f, 0.f, 0.f });
			physics_system->GetBodyInterface().SetGravityFactor(phys.physicsBody, 1.f);
		});
		physics_system->OptimizeBroadPhase();
		//physics_system->SaveState(myRecorder);
		SHARD3D_INFO("Activated physics");
	}
	void PhysicsSystem::end(ECS::Level* level) {
		//physics_system->RestoreState(myRecorder);
		SHARD3D_INFO("Deactivated physics");
	}
#define KILLZ -1000.f
	void PhysicsSystem::simulate(ECS::Level* level, float frameTime) {
		// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
		const int cCollisionSteps = 1;

		// If you want more accurate step results you can do multiple sub steps within a collision step. Usually you would set this to 1.
		const int cIntegrationSubSteps = 1;
		physics_system->Update(frameTime, cCollisionSteps, cIntegrationSubSteps, temp_allocator.get(), jobsystem.get());
		level->registry.view<Components::RigidbodyComponent, Components::TransformComponent>().each([&](Components::RigidbodyComponent& phys, Components::TransformComponent& transform) {
			
			JPH::Vec3 translation = physics_system->GetBodyInterface().GetTransformedShape(phys.physicsBody).GetWorldTransform().GetTranslation();
			JPH::Vec3 rotation = physics_system->GetBodyInterface().GetTransformedShape(phys.physicsBody).GetWorldTransform().GetRotationSafe().GetQuaternion().GetXYZ();
			SHARD3D_LOG("Physics Actor {0} new transform [ {1}, {2}, {3} ]", phys.physicsBody.GetIndex(), translation.GetX(), translation.GetZ(), translation.GetY());
			
			// Override matrix (scale isnt taken into account & normal matrices dont work either)
			// 
			JPH::Mat44 base = physics_system->GetBodyInterface().GetTransformedShape(phys.physicsBody).GetWorldTransform();
			glm::mat4 converted{ 1.f };
			converted[0] = { base.GetColumn4(0).GetX(), base.GetColumn4(0).GetY(), base.GetColumn4(0).GetZ(), base.GetColumn4(0).GetW() };
			converted[1] = { base.GetColumn4(1).GetX(), base.GetColumn4(1).GetY(), base.GetColumn4(1).GetZ(), base.GetColumn4(1).GetW() };
			converted[2] = { base.GetColumn4(2).GetX(), base.GetColumn4(2).GetY(), base.GetColumn4(2).GetZ(), base.GetColumn4(2).GetW() };
			converted[3] = { base.GetColumn4(3).GetX(), base.GetColumn4(3).GetY(), base.GetColumn4(3).GetZ(), base.GetColumn4(3).GetW() };
			transform.transformMatrix = glm::scale(converted, { transform.getScale().x,transform.getScale().z, transform.getScale().y });
			transform.normalMatrix = glm::transpose(glm::inverse(transform.transformMatrix));

			if (translation.GetY() < KILLZ) {
				SHARD3D_LOG("Physics Actor {0} is out of bounds by {1} meters!", phys.physicsBody.GetIndex(), transform.getTranslation().z - KILLZ);
			}
		});
	}
	JPH::ShapeRefC PhysicsSystem::createBoxShape(const JPH::BoxShapeSettings& settings) {
		JPH::ShapeSettings::ShapeResult result = settings.Create();
		JPH::ShapeRefC shape = result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
		if (result.IsValid())
			return shape;
		else
			SHARD3D_FATAL(result.GetError().c_str());
	}
	JPH::ShapeRefC  PhysicsSystem::createSphereShape(const JPH::SphereShapeSettings& settings) {
		JPH::ShapeSettings::ShapeResult result = settings.Create();
		JPH::ShapeRefC shape = result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
		if (result.IsValid())
			return shape;
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
	
	JPH::BodyID PhysicsSystem::createBody(const JPH::BodyCreationSettings& settings, bool activate) {
		JPH::Body* body = physics_system->GetBodyInterface().CreateBody(settings);
		JPH::BodyID bodyID = body->GetID();
		physics_system->GetBodyInterface().AddBody(bodyID, activate? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
		return bodyID;
	}
	
	void PhysicsSystem::dumpShapeToBin(JPH::Ref<JPH::Shape> shape, const std::string& path) {
	
	}

}
