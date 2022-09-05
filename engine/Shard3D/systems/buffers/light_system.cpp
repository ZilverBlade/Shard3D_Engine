#include "../../s3dpch.h" 
#include "../../ecs.h"
#include "light_system.h"

namespace Shard3D {
	void LightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {	
		{
			int lightIndex = 0;
			frameInfo.activeLevel->registry.view<Components::PointlightComponent, Components::TransformComponent>().each([&](Components::PointlightComponent light, Components::TransformComponent transform) {
				ubo.pointlights[lightIndex].position = transform.transformMatrix[3];
				ubo.pointlights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
				ubo.pointlights[lightIndex].attenuationMod = glm::vec4(light.attenuationMod, 0.f);
				ubo.pointlights[lightIndex].specularMod = light.specularMod;
				ubo.pointlights[lightIndex].radius = light.radius;
				lightIndex += 1;
			});
			ubo.numPointlights = lightIndex;
		}
		{
			int lightIndex = 0;
			frameInfo.activeLevel->registry.view<Components::SpotlightComponent, Components::TransformComponent>().each([&](Components::SpotlightComponent light, Components::TransformComponent transform) {
				glm::vec3 r = transform.getRotation();
				glm::vec3 t = transform.getTranslation();

				ubo.spotlights[lightIndex].position = transform.transformMatrix[3];
				ubo.spotlights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
				ubo.spotlights[lightIndex].direction = glm::vec4(r.x, r.z, r.y, 1.f);
				ubo.spotlights[lightIndex].angle = glm::vec2(light.outerAngle, light.innerAngle);
				ubo.spotlights[lightIndex].attenuationMod = glm::vec4(light.attenuationMod, 0.f);
				ubo.spotlights[lightIndex].specularMod = light.specularMod;
				ubo.spotlights[lightIndex].radius = light.radius;
				lightIndex += 1;
			});
			ubo.numSpotlights = lightIndex;
		}
		{
			int lightIndex = 0;
			frameInfo.activeLevel->registry.view<Components::DirectionalLightComponent, Components::TransformComponent>().each([&](Components::DirectionalLightComponent light, Components::TransformComponent transform) {
				glm::vec3 r = transform.getRotation();
				glm::vec3 t = transform.getTranslation();

				ubo.directionalLights[lightIndex].position = transform.transformMatrix[3];
				ubo.directionalLights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
				ubo.directionalLights[lightIndex].direction = glm::vec4(r.x, r.z, r.y, 1.f);
				ubo.directionalLights[lightIndex].specularMod = light.specularMod;
			//	ubo.directionalLights[lightIndex].lightProjection = light.lightProjection;
				lightIndex += 1;
			});
			ubo.numDirectionalLights = lightIndex;
		}
	}
}