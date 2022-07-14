#include "../s3dtpch.h"
#include "particle_system.hpp"

namespace Shard3D {
	ParticleSystem::ParticleSystem() {
		particlePool.resize(1000);
	}
	ParticleSystem::~ParticleSystem() {

	}
	void ParticleSystem::update(FrameInfo& frameInfo) {

	}

	void ParticleSystem::render(FrameInfo& frameInfo, std::shared_ptr<wb3d::Level>& level) {

	}
	void ParticleSystem::emit(ParticleProperties& properties) {
		Particle& particle = particlePool[poolIndex];
		particle.active = true;
		particle.velocity = properties.velocity;
		particle.velocity += properties.velocityVar;

	}
}