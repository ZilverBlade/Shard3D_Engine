#include "shard_game_object.hpp"

namespace shard {
	glm::mat4 TransformComponent::mat4() {

		// Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
		// Rotation convention = YXZ tait-bryan angles
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix

		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f} };
	}

	glm::mat3 TransformComponent::normalMatrix() {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		const glm::vec3 invScale = 1.0f / scale;
		return glm::mat3{
			{
				invScale.x * (c1 * c3 + s1 * s2 * s3),
				invScale.x * (c2 * s3),
				invScale.x * (c1 * s2 * s3 - c3 * s1),
			},
			{
				invScale.y * (c3 * s1 * s2 - c1 * s3),
				invScale.y * (c2 * c3),
				invScale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				invScale.z* (c2 * s1),
				invScale.z* (-s2),
				invScale.z * (c1 * c2),
			},};
	}
	ShardGameObject ShardGameObject::makePointlight(float intensity, float radius, glm::vec3 color, glm::vec3 attenuationMod) {
		ShardGameObject gameObj = ShardGameObject::createGameObject(); 
		gameObj.color = color;
		gameObj.transform.scale.x = radius;
		gameObj.pointlight = std::make_unique<PointlightComponent>();
		gameObj.pointlight->lightIntensity = intensity;
		gameObj.pointlight->attenuationMod = glm::vec4(attenuationMod, 0.f);
		return gameObj;
	}
	ShardGameObject ShardGameObject::makeSpotlight(float intensity, float radius, glm::vec3 color, glm::vec3 direction, float outerAngle, float innerAngle, glm::vec3 attenuationMod) {
		ShardGameObject gameObj = ShardGameObject::createGameObject();
		gameObj.color = color;
		gameObj.transform.scale.x = radius;
		gameObj.spotlight = std::make_unique<SpotlightComponent>();
		gameObj.spotlight->lightIntensity = intensity;
		gameObj.transform.rotation = direction;
		gameObj.spotlight->outerAngle = outerAngle;
		gameObj.spotlight->innerAngle = innerAngle;
		gameObj.spotlight->attenuationMod = glm::vec4(attenuationMod, 0.f);
		return gameObj;
	}
	ShardGameObject ShardGameObject::makeDirectionalLight(float intensity, glm::vec3 color, glm::vec3 direction)
	{
		ShardGameObject gameObj = ShardGameObject::createGameObject();
		gameObj.color = color;
		gameObj.directionalLight = std::make_unique<DirectionalLightComponent>();
		gameObj.directionalLight->lightIntensity = intensity;
		gameObj.transform.rotation = direction;
		return gameObj;
	}
}