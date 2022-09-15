#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <entt.hpp>

#include "../../core/asset/assetmgr.h"
#include "../../core/misc/UUID.h"
#include "../../core/asset/assetid.h"
#include "../rendering/camera.h"

#include "../audio/audio.h"
#include "../../systems/computational/particle_system.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Shard3D {
	class LevelPropertiesPanel;
	namespace Components {
		struct UUIDComponent {
			inline UUID getID() { return id; }
			UUIDComponent(UUID id_) { id = id_; }
			UUIDComponent(const UUIDComponent&) = default;
		private:
			UUID id;
		};

		struct TagComponent {
			std::string tag;

			TagComponent() = default;
			TagComponent(const TagComponent&) = default;
			operator std::string() { return tag; };
		};

		struct RelationshipComponent {
			entt::entity parentActor{ entt::null };
			std::vector<entt::entity> childActors{};

			RelationshipComponent() = default;
			RelationshipComponent(const RelationshipComponent&) = default;
		};

		struct TransformComponent {
			void setTranslation(glm::vec3 _t) { dirty = true; translation = glm::vec3(_t.x, _t.z, _t.y); }
			void setRotation(glm::vec3 _r) { dirty = true; rotation = glm::vec3(_r.x, _r.z, _r.y); }
			void setScale(glm::vec3 _s) { dirty = true; scale = glm::vec3(_s.x, _s.z, _s.y); }

			void setTranslationX(float val) { dirty = true; translation.x = val; }
			void setRotationX(float val) { dirty = true; rotation.x = val; }
			void setScaleX(float val) { dirty = true; scale.x = val; }
			void setTranslationY(float val) { dirty = true; translation.z = val; }
			void setRotationY(float val) { dirty = true; rotation.z = val;}
			void setScaleY(float val) { dirty = true; scale.z = val; }
			void setTranslationZ(float val) { dirty = true; translation.y = val; }
			void setRotationZ(float val) { dirty = true; rotation.y = val; }
			void setScaleZ(float val) { dirty = true; scale.y = val; }

			glm::vec3 getTranslation() { return glm::vec3(translation.x, translation.z, translation.y); }
			glm::vec3 getRotation() { return glm::vec3(rotation.x, rotation.z, rotation.y); }
			glm::vec3 getScale() { return glm::vec3(scale.x, scale.z, scale.y); }

			static void decompose(const glm::mat4& transform, glm::vec3* outTranslation, glm::vec3* outRotationEulerXYZ, glm::vec3* outScale);

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;

			glm::mat4 transformMatrix{1.f};
			glm::mat3 normalMatrix{1.f};

			glm::mat4 calculateMat4();
			glm::mat3 calculateNormalMatrix();

			void setGlobalTranslationDirect(glm::vec3 _t) {
				translation = glm::vec3(_t.x, _t.z, _t.y); 
				transformMatrix = calculateMat4(); 
				normalMatrix = calculateNormalMatrix();
			}

			void setGlobalRotationDirect(glm::vec3 _r) {
				rotation = glm::vec3(_r.x, _r.z, _r.y);
			}

			void setGlobalScaleDirect(glm::vec3 _s) {
				scale = glm::vec3(_s.x, _s.z, _s.y);
			}

			void recalculateMatrix() {
				transformMatrix = calculateMat4();
				normalMatrix = calculateNormalMatrix();
			}

			bool dirty = true;
		private:
			glm::vec3 translation{ 0.f, 0.f, 0.f };
			glm::vec3 rotation{ 0.f, 0.f, 0.f };
			glm::vec3 scale{ 1.f, 1.f, 1.f };
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
			std::vector<PostProcessingMaterialInstance> postProcessMaterials{};
		private:
			float fov = 70.f;
			float nearClip = 0.05f;
			float farClip = 1024.f;
			ProjectType projectionType = ProjectType::Perspective;
			
			friend class LevelPropertiesPanel;
		};
		
		struct Mesh3DComponent {
			AssetID asset{ "" };
			std::vector<AssetID> materials;
			// resets materials to be whatever mesh is loaded if the material size doesnt match the asset
			void validate();
			bool hideInGame = false;

			Mesh3DComponent() = default;
			Mesh3DComponent(const Mesh3DComponent&) = default;
			Mesh3DComponent(const AssetID& mdl);
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
		struct Rigidbody3DComponent {
			enum class PhysicsState {
				STATIC,
				DYNAMIC,
				KINEMATIC 
			};

			float mass;
			float friction;
			float restitution;

			JPH::BodyID physicsBody{};

			PhysicsState state = PhysicsState::STATIC;
		};
		struct TriggerVolumeComponent {
			
		};

		struct ExponentialFogComponent {
			//https://iquilezles.org/articles/fog/
		};
		struct AreaFogComponent {
			// allows for foggy lights
		};
		struct DistanceMistComponent {
			//https://developer.download.nvidia.com/CgTutorial/cg_tutorial_chapter09.html chaper 9.1.5 and https://vicrucann.github.io/tutorials/osg-shader-fog/
		};
		struct DecalComponent {

		};
		struct ReflectionCubeComponent {

		};
	}
}