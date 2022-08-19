#pragma once
#include "../rendering/camera.h"

#include "../../core.h"
#include "../vulkan_api/descriptors.h"
#include "../asset/texture.h"
#include "../ecs/level.h"

namespace Shard3D {
	// fix this shit up eventually, only temporary solution
	struct Pointlight {
		glm::vec4 position{};
		glm::vec4 color{}; 
		glm::vec4 attenuationMod{};
		alignas(16)float specularMod{};
		float radius{};
	};
	struct Spotlight {
		glm::vec4 position{};
		glm::vec4 color{};
		glm::vec4 direction{};
		alignas(16) glm::vec2 angle{}; //x = outer, y = inner
		glm::vec4 attenuationMod{};
		alignas(16)float specularMod{};
		float radius{};
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
		glm::vec4 ambientColor = { 1.0f, 1.0f, 1.0f, 0.01f };

		Pointlight pointlights[ENGINE_MAX_POINTLIGHTS];
		Spotlight spotlights[ENGINE_MAX_SPOTLIGHTS];
		DirectionalLight directionalLights[ENGINE_MAX_DIRECTIONAL_LIGHTS];
		int numPointlights;
		int numSpotlights;
		int numDirectionalLights;
		
		alignas(16)glm::vec3 materialSettings= {1.f, 0.5f, 0.f};
	};

	struct ComputeUbo {
		VkSampler inputImage;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		EngineCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		EngineDescriptorPool& perDrawDescriptorPool;  // pool of descriptors that is cleared each frame
		sPtr<ECS::Level> activeLevel;
	};
}