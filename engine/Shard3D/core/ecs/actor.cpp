#include "../../s3dpch.h"
#include "actor.h"

namespace Shard3D {
	namespace ECS {
		Actor::Actor(entt::entity _handle, Level* _level) : actorHandle(_handle), level(_level) {}
	}
}