#include "blueprint.hpp"

namespace Shard3D {
	namespace wb3d {

		Blueprint::Blueprint(Actor actor, std::string path, std::string name) : container(actor), assetFile(path), name(name) { 
			ID = GUID(); // init GUID
		}

	}
}