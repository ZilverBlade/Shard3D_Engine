#include "singleton.hpp"
#include "wb3d/assetmgr.hpp"
namespace Shard3D {
	Destructor::~Destructor() { 
		wb3d::AssetManager::clearAllAssets(); 
		SharedPools::destructPools();
		Singleton::viewportImage = nullptr;
	}
}