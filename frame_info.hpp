#pragma once

#include "camera.hpp"
#include "components.hpp"
#include <vulkan/vulkan.h>
#include "utils/definitions.hpp"

namespace Shard3D {

	struct Pointlight {
		glm::vec4 position{};
		glm::vec4 color{}; 
		glm::vec4 attenuationMod{};
		alignas(16)float specularMod{};
	};
	struct Spotlight {
		glm::vec4 position{};
		glm::vec4 color{};
		glm::vec4 direction{};
		alignas(16) glm::vec2 angle{}; //x = outer, y = inner
		glm::vec4 attenuationMod{};
		alignas(16)float specularMod{};
	};
	struct DirectionalLight {
		glm::vec4 position{};
		glm::vec4 color{};
		glm::vec4 direction{};
		alignas(16)float specularMod{};
	};

	struct GlobalUbo {
		//alignment rules https://cdn.discordapp.com/attachments/763044823342776340/974725640274210926/unknown.png

		/*
			For colour parameters use "w" for intensity
		*/

		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };

		//reyleigh scattering fakery and/or indirect light
		glm::vec4 ambientColor = { 0.8f, 0.9f, 1.f, 0.54f };

		Pointlight pointlights[MAX_POINTLIGHTS];
		Spotlight spotlights[MAX_SPOTLIGHTS];
		DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
		int numPointlights;
		int numSpotlights;
		int numDirectionalLights;
	};


	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		EngineCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}