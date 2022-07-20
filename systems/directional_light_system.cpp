#include "../s3dtpch.h" 
#include "directional_light_system.hpp"

namespace Shard3D {
	void DirectionalLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
		int lightIndex = 0;
		level->registry.view<Components::DirectionalLightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {
			ubo.directionalLights[lightIndex].position = glm::vec4(transform.translation, 1.f);
			ubo.directionalLights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
			ubo.directionalLights[lightIndex].direction = glm::vec4(transform.rotation, 1.f);
			ubo.directionalLights[lightIndex].specularMod = light.specularMod;
			lightIndex += 1;
		});
		ubo.numDirectionalLights = lightIndex;
	}
}