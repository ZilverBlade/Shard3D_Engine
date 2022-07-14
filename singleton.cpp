#include "s3dtpch.h"
#include "singleton.hpp"
#include "wb3d/assetmgr.hpp"

namespace Shard3D {
	Destructor::~Destructor() {
		DynamicScriptEngine::destroy();
	}
}