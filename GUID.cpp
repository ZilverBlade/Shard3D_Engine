#include "GUID.hpp"

#include <random>
namespace Shard3D {

	static std::random_device GUID_randomDevice;
	static std::mt19937_64 GUID_engine(GUID_randomDevice());
	static std::uniform_int_distribution<uint64_t> GUID_uniformDistribution;

	GUID::GUID() : guid(GUID_uniformDistribution(GUID_engine)) {

	}

	GUID::GUID(uint64_t overrideGUID) : guid(overrideGUID) {

	}

	GUID::~GUID() {

	}

}