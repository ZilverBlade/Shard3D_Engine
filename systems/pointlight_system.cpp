#include "../s3dtpch.h" 

#include "pointlight_system.hpp"

namespace Shard3D {
	void PointlightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level) {
		int lightIndex = 0;
		level->registry.view<Components::PointlightComponent, Components::TransformComponent>().each([&](auto light, auto transform) {		
			glm::vec3 t = transform.getTranslation();

			ubo.pointlights[lightIndex].position = glm::vec4(t.x, t.z, t.y, 1.f);
			ubo.pointlights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);
			ubo.pointlights[lightIndex].attenuationMod = glm::vec4(light.attenuationMod, 0.f);
			ubo.pointlights[lightIndex].specularMod = light.specularMod;
			lightIndex += 1;
		});
		ubo.numPointlights = lightIndex;
	}
}