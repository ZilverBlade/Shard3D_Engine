#include "../s3dtpch.h" 

#include "spotlight_system.hpp"
namespace Shard3D {
	void SpotlightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
		int lightIndex = 0;
		level->registry.view<Components::SpotlightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {
			ubo.spotlights[lightIndex].position = glm::vec4(transform.translation, 1.f);
			ubo.spotlights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
			ubo.spotlights[lightIndex].direction = glm::vec4(transform.rotation, 1.f);
			ubo.spotlights[lightIndex].angle = glm::vec2(light.outerAngle, light.innerAngle);
			ubo.spotlights[lightIndex].attenuationMod = glm::vec4(light.attenuationMod, 0.f);
			ubo.spotlights[lightIndex].specularMod = light.specularMod;
			lightIndex += 1;
		});
		ubo.numSpotlights = lightIndex;
	}
}