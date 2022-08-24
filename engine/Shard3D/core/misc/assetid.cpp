#include "assetid.h"
#include <xhash>
namespace Shard3D {
	static std::hash<std::string> hasher;
	AssetID::AssetID(const std::string& asset) : assetFile(asset), assetID(hasher(asset)) {}
	AssetID::AssetID(const uint64_t& id) : assetFile("<IDKEY>::NOFILE"), assetID(id) {}
}