#pragma once
#include <sys/stat.h>
#include <string>
namespace Shard3D {
	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	template <class T>
	static void SafeRelease(T** ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}
	
	class strUtils {
	public:
		static inline bool hasEnding(std::string const& fullString, std::string const& ending) {
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		static inline bool hasStarting(std::string const& fullString, std::string const& starting) {
			return (fullString.rfind(starting, 0) == 0);
		}
	};
} 

#define BIT(x) (1 << x)