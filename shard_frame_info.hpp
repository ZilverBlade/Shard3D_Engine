#pragma once

#include "camera.hpp"
#include "shard_game_object.hpp"
#include <vulkan/vulkan.h>

namespace shard {

	#define MAX_LIGHTS 20 //engine setting

	struct Pointlight {
		glm::vec4 position{}; //ignore w
		glm::vec4 color{}; 
		glm::vec4 attenuationMod{};
	};

	struct GlobalUbo {
		//alignment rules https://cdn.discordapp.com/attachments/763044823342776340/974725640274210926/unknown.png

		/*
			For colour parameters use "w" for intensity
		*/

		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };

		//reyleigh scattering fakery and/or indirect light
		glm::vec4 ambientColor = { 0.8f, 0.9f, 1.f, 0.008f };

		//directional light
		//alignas(16) glm::vec3 directionalLightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
		
		Pointlight pointlights[MAX_LIGHTS];
		int numLights;
		//spotlight?
	};


	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		ShardCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		ShardGameObject::Map& gameObjects;
	};
}