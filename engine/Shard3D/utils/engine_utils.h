#pragma once
#include <sys/stat.h>
#include <string>
#include <vector>
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
			if ((fullString.length() < ending.length())) return false;
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		static inline bool hasStarting(std::string const& fullString, std::string const& starting) {
			return (fullString.rfind(starting, 0) == 0);
		}
	};

	class IOUtils {
	public:
		static void writeStackBinary(void* data, size_t object_size, const std::string& path);
		static std::vector<uint8_t> getStackBinary(void* data, size_t object_size);
		static void* readBinary(const std::string& path);
	};
} 

#define BIT(x) (1 << x)