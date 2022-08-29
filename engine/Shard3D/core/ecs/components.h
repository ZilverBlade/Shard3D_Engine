#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <entt.hpp>

#include "../../core/asset/assetmgr.h"
#include "../../core/misc/UUID.h"
#include "../../core/asset/assetid.h"
#include "../rendering/camera.h"

#include "../audio/audio.h"
#include "../../systems/computational/particle_system.h"

namespace Shard3D {
	class LevelPropertiesPanel;
	namespace Components {
		struct UUIDComponent {
			UUID id;

			UUIDComponent() = default;
			UUIDComponent(const UUIDComponent&) = default;
		};

		struct TagComponent {
			std::string tag;

			TagComponent() = default;
			TagComponent(const TagComponent&) = default;
			operator std::string() { return tag; };
		};

		struct TransformComponent {
			void setTranslation(glm::vec3 _t) { translation = glm::vec3(_t.x, _t.z, _t.y); }
			void setRotation(glm::vec3 _r) { rotation = glm::vec3(_r.x, _r.z, _r.y); }
			void setScale(glm::vec3 _s) { scale = glm::vec3(_s.x, _s.z, _s.y); }

			void setTranslationX(float val) { translation.x = val; }
			void setRotationX(float val) { rotation.x = val; }
			void setScaleX(float val) { scale.x = val; }
			void setTranslationY(float val) { translation.z = val; }
			void setRotationY(float val) { rotation.z = val;}
			void setScaleY(float val) { scale.z = val; }
			void setTranslationZ(float val) { translation.y = val; }
			void setRotationZ(float val) { rotation.y = val; }
			void setScaleZ(float val) { scale.y = val; }

			glm::vec3 getTranslation() { return glm::vec3(translation.x, translation.z, translation.y); }
			glm::vec3 getRotation() { return glm::vec3(rotation.x, rotation.z, rotation.y); }
			glm::vec3 getScale() { return glm::vec3(scale.x, scale.z, scale.y); }

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;

			glm::mat4 mat4();
			glm::mat3 normalMatrix();
		private:
			glm::vec3 translation{ 0.f, 0.f, 0.f };
			glm::vec3 rotation{ 0.f, 0.f, 0.f };
			glm::vec3 scale{ 1.f, 1.f, 1.f };
			friend class LevelPropertiesPanel; // to modify the values real time
			friend struct MatrixCalculator;
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
			void setProjectionType(ProjectType _pt) { projectionType = _pt; setProjection(); };
			ProjectType getProjectionType() { return projectionType; };
			/* *
* Vertical Field of view (degrees)
*/
			void setFOV(float _f) { fov = _f; camera.setPerspectiveProjection(glm::radians(fov), ar, nearClip, farClip); }
			float getFOV() { return fov; }
			/* *
* Near clip distance (meters)
*/
			void setNearClip(float _d) { nearClip = _d; setProjection(); }
			float getNearClip() { return nearClip; }
			/* *
* Far clip distance (meters)
*/
			void setFarClip(float _d) { farClip = _d; setProjection();}
			float getFarClip() { return farClip; }
			/* *
* Aspect ratio (width/height)
*/
			float ar = 16 / 9;
			/* *
* Set projection with the given settings
*/	
			operator EngineCamera&() {
				return camera;
			}
			CameraComponent() = default;
			CameraComponent(const CameraComponent&) = default;
			void setProjection() {
				if (projectionType == ProjectType::Perspective)
					camera.setPerspectiveProjection(glm::radians(fov), ar, nearClip, farClip);
				else  camera.setOrthographicProjection(-ar, ar, -1, 1, nearClip, farClip);
			}
		private:
			float fov = 70.f;
			float nearClip = 0.05f;
			float farClip = 1024.f;
			ProjectType projectionType = ProjectType::Perspective;
			
			friend class LevelPropertiesPanel;
		};
		
		struct MeshComponent {
			AssetID asset{ "" };
			std::vector<AssetID> materials;

			bool hideInGame = false;

			MeshComponent() = default;
			MeshComponent(const MeshComponent&) = default;
			MeshComponent(const AssetID& mdl);
		};

		struct BillboardComponent {
			AssetID asset{ "" };
			
			bool hideInGame = false;

			enum class BillboardOrientation {
				SCREEN_VIEW_ALIGNED,
				VIEW_POINT_ALIGNED,
				AXIAL,
			};
			// SCREEN_VIEW_ALIGNED is the only supported orientation at the moment
			BillboardOrientation orientation = BillboardOrientation::SCREEN_VIEW_ALIGNED;

			BillboardComponent() = default;
			BillboardComponent(const BillboardComponent&) = default;
			BillboardComponent(const AssetID& tex);
		};

		struct AudioComponent {
		private:
			EngineAudio* audioEngine{};
		public:
			std::string file{};
			AudioProperties properties{};
			
			void play() {
				audioEngine = new EngineAudio();
				audioEngine->play(file);
				audioEngine->update(properties);
			}
			void pause() {
				audioEngine->pause();
			}
			void resume() {
				audioEngine->resume();
			}
			void update() {
				audioEngine->update(properties);
			}
			void stop() {
				audioEngine->stop();
				delete audioEngine;
			}
			AudioComponent() = default;
			AudioComponent(const AudioComponent&) = default;
		};
		struct PointlightComponent {
			float radius = 1.f;
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			glm::vec3 attenuationMod = { 0.f, 0.f, 1.f };
			float specularMod = 1.0f;

			PointlightComponent() = default;
			PointlightComponent(const PointlightComponent&) = default;
		};

		struct SpotlightComponent {
			float radius = 1.f;
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float outerAngle = glm::radians(60.0f);
			float innerAngle = glm::radians(30.0f);
			glm::vec3 attenuationMod = { 0.f, 0.f, 1.f };
			float specularMod = 1.0f;

			SpotlightComponent() = default;
			SpotlightComponent(const SpotlightComponent&) = default;
		};

		struct DirectionalLightComponent {
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float lightIntensity = 1.0f;
			float specularMod = 1.0f;
			glm::mat4 lightProjection{ 1.f };

			DirectionalLightComponent() = default;
			DirectionalLightComponent(const DirectionalLightComponent&) = default;
		};
		struct ScriptComponent {
			std::string name = "Example";
			int lang = 0; // C# = 0, VB = 1
			ScriptComponent() = default;
			ScriptComponent(const ScriptComponent&) = default;
		};
		struct ParticleComponent {
			ParticleProperties particleTemplate;
			uint16_t maxParticles;
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
		};
	}
}