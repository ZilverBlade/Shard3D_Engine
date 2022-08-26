#pragma once
#include <string>

namespace Shard3D {
	using AssetKey = unsigned long long;

	class AssetID {
	public:
		AssetID(const std::string& asset);
		// Returned hashed value
		inline uint64_t getID() const { return assetID; }
		// Returns asset file (.s3dasset)
		inline std::string getFile() const { return assetFile; }
		// Returns reference to asset file to modify, as well as permanently marks as dirty since the asset no longer matches the hash
		inline std::string& getFileRef() { return assetFile; dirty = true; }

		inline operator AssetKey() const { return getID(); }
		inline operator std::string() const { return getFile(); }
	private:
		bool dirty = false;
		std::string assetFile;
		uint64_t assetID;
	};

}