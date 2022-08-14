#include "../../s3dpch.h"
#include "blueprint.h"

namespace Shard3D {
	namespace ECS {
		Blueprint::Blueprint(Actor actor, std::string path, std::string name) : container(actor), assetFile(path), name(name) { 
			ID = UUID();
		}
	}
}