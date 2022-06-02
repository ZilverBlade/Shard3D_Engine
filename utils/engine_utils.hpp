#pragma once
#include <sys/stat.h>

namespace Shard3D {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	/*
	//no checks or anything, so can only be used by core functions that know what type of value it returns
	auto getValFromEngineConfig(const char* header, const char* key) {
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);
		return ini.GetValue(header, key);
	};
	auto getValFromGameConfig(const char* header, const char* key) {
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(GAME_SETTINGS_PATH);
		return ini.GetValue(header, key);
	};
	*/
} 
