#pragma once
#include "s3dtpch.h"
#include "camera.hpp"

#include "utils/definitions.hpp"
#include "descriptors.hpp"
#include "texture.hpp"

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
		glm::vec4 ambientColor = { 0.8f, 0.9f, 1.f, 0.014f };

		Pointlight pointlights[ENGINE_MAX_POINTLIGHTS];
		Spotlight spotlights[ENGINE_MAX_SPOTLIGHTS];
		DirectionalLight directionalLights[ENGINE_MAX_DIRECTIONAL_LIGHTS];
		int numPointlights;
		int numSpotlights;
		int numDirectionalLights;
	};

	struct ComputeUbo {
		VkSampler inputImage;
	};

	struct SurfaceLitMaterialUbo {
		VkSampler normalTex{};

		//glm::vec4 emissiveColor{};
		//VkSampler emissiveTex{};

		glm::vec4 diffuseColor{};
		VkSampler diffuseTex{};

		alignas(16) float specular{};
		VkSampler specularTex{};

		alignas(16) float roughness{};
		VkSampler roughnessTex{};

		alignas(16) float metallic{};
		VkSampler metallicTex{};

		VkSampler maskTex{};
	};
	struct SurfaceUnlitMaterialUbo {
		glm::vec4 emissiveColor{};
		VkSampler emissiveTex{};

		VkSampler maskTex{};
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		EngineCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		EngineDescriptorPool& perDrawDescriptorPool;  // pool of descriptors that is cleared each frame
	};
}