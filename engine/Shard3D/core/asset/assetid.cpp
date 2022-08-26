#include "assetid.h"
#include <xhash>
#include <algorithm>
namespace Shard3D {
	static std::hash<std::string> hasher;
	static std::string& convert(std::string& orig) { std::replace(orig.begin(), orig.end(), '\\', '/'); return orig; }
	AssetID::AssetID(const std::string& asset) : assetFile(convert(const_cast<std::string&>(asset))), assetID(hasher(convert(const_cast<std::string&>(asset)))) {}
}