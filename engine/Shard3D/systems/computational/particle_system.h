#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/ecs/level.h"
namespace Shard3D {
	struct ParticleProperties {
		glm::vec3 position;
		glm::vec4 velocity, velocityVar;//beginVelocity, endVelocity, varVelocity;
		glm::vec4 beginColor, endColor;
		glm::vec2 beginSize, endSize, varSize;

		float lifeTime = 10.f;
	};
	
	class ParticleSystem {
		struct Particle {
			glm::vec3 position;
			glm::vec4 velocity;
			glm::vec4 beginColor, endColor;
			glm::vec2 beginSize, endSize;
			float rotation;
			float lifeTime = 10.f;
			float timeRemaining;
			bool active = false;
		};
		
	public:
		ParticleSystem();
		~ParticleSystem();

		void update(FrameInfo& frameInfo);
		void render(FrameInfo& frameInfo);
		void emit(ParticleProperties& properties);
	private:
		std::vector<Particle> particlePool;
		uint32_t poolIndex = 999;
	};
}