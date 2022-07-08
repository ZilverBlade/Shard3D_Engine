#pragma once
#include "s3dtpch.h"
#include "model.hpp"
#include "GUID.hpp"

#include "../../camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <entt.hpp>

#include "texture.hpp"
#include <Jolt.h>
#include <Physics/Body/Body.h>
#include <Physics/Collision/Shape/Shape.h>

namespace Shard3D {
	namespace wb3d {
		class Blueprint;
	}
	class wb3d::Blueprint;

	namespace Components {
		struct GUIDComponent {
			GUID id;

			GUIDComponent() = default;
			GUIDComponent(const GUIDComponent&) = default;
		};

		struct TagComponent {
			std::string tag;

			TagComponent() = default;
			TagComponent(const TagComponent&) = default;
			operator std::string() { return tag; };
		};

		struct BlueprintComponent {
			wb3d::Blueprint* blueprint;
		};

		struct TransformComponent {
			glm::vec3 translation{ 0.f, 0.f, 0.f };
			glm::vec3 scale{ 1.f, 1.f, 1.f };
			glm::vec3 rotation{ 0.f, 0.f, 0.f };
			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;

			glm::mat4 mat4();
			glm::mat3 normalMatrix();
		};

		struct CameraComponent {
			enum ProjectType {
				Orthographic = 0,
				Perspective = 1
			};

			EngineCamera camera{};
			/* *
* Projection type (Perspective/Orthographic)
*/
			ProjectType projectionType = ProjectType::Perspective;
			/* *
* Vertical Field of view (degrees)
*/
			float fov = 70.f;
			/* *
* Near clip distance (meters)
*/
			float nearClip = 0.05f;
			/* *
* Far clip distance (meters)
*/
			float farClip = 1024.f;
			/* *
* Aspect ratio (width/height)
*/
			float ar = 16 / 9;
			/* *
* Set projection with the given settings
*/
			void setProjection() {
				if (projectionType == ProjectType::Perspective)
				camera.setPerspectiveProjection(glm::radians(fov), ar, nearClip, farClip);
				else  camera.setOrthographicProjection(-ar, ar, -1, 1, nearClip, farClip);
			}

			operator EngineCamera&() {
				return camera;
			}
		};
		struct BillboardComponent {
			std::string file{};
			std::string cacheFile{};
			enum class BillboardOrientation {
				NONE = 0,
				AXIAL = 1,
				POINT = 2
			};
			// BILLBOARD_ORIENTATION_AXIAL is the only supported orientation at the moment
			BillboardOrientation orientation = BillboardOrientation::AXIAL;

			BillboardComponent() = default;
			BillboardComponent(const BillboardComponent&) = default;
			BillboardComponent(const std::string& tex);
			void reapplyTexture(const std::string& tex);
		};
		struct MeshComponent {
			std::string file{};
			std::string cacheFile{};
			MeshType type = MeshType::MESH_TYPE_NULL;
			MaterialSystem::MaterialList materialList;
			bool hideInGame = false;

			MeshComponent() = default;
			MeshComponent(const MeshComponent&) = default;
			MeshComponent(const std::string& mdl);
			void reapplyMesh(const std::string& mdl);
		};

		struct PointlightComponent {
			float radius = 1.f;
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			glm::vec3 attenuationMod = { 0.f, 0.f, 1.f };
			float specularMod = 1.0f;
		};

		struct SpotlightComponent {
			float radius = 1.f;
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float outerAngle = glm::radians(15.0f);
			float innerAngle = glm::radians(45.0f);
			glm::vec3 attenuationMod = { 0.f, 0.f, 1.f };
			float specularMod = 1.0f;
		};

		struct DirectionalLightComponent {
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float specularMod = 1.0f;
		};

		struct RigidbodyComponent {
			enum class PhysicsState {
				NONE = 0,
				STATIC = 1,
				DYNAMIC = 2,
				KINEMATIC = 3
			};

			float mass;
			float friction;
			float restitution;

			PhysicsState state = PhysicsState::STATIC;
			std::shared_ptr<JPH::Shape> shape;
		};
	}
}