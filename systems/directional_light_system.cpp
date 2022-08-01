#include "../s3dtpch.h" 
#include "directional_light_system.hpp"

namespace Shard3D {
	void DirectionalLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
		int lightIndex = 0;
		level->registry.view<Components::DirectionalLightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {
			glm::vec3 r = transform.getRotation();
			glm::vec3 t = transform.getTranslation();

			ubo.directionalLights[lightIndex].position = glm::vec4(t.x, t.z, t.y, 1.f);
			ubo.directionalLights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
			ubo.directionalLights[lightIndex].direction = glm::vec4(r.x, r.z, r.y, 1.f);
			ubo.directionalLights[lightIndex].specularMod = light.specularMod;
			lightIndex += 1;
		});
		ubo.numDirectionalLights = lightIndex;
	}
}