#include "actor.hpp"

namespace Shard3D {
	namespace wb3d {

		Actor::Actor(entt::entity handle, Scene* scene) : actorHandle(handle), eScene(scene){ }

		Actor::~Actor() { }

	}
}