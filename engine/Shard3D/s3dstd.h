#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace Shard3D {
#pragma region Smart Pointers
	// Unique Pointer
	template<typename T>
	using uPtr = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr inline std::unique_ptr<T> make_uPtr(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// Shared Pointer
	template<typename T>
	using sPtr = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr inline std::shared_ptr<T> make_sPtr(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// Resource Pointer
	template<typename T>
	using rPtr = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr inline std::shared_ptr<T> make_rPtr(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// Weak Pointer
	template<typename T>
	using wPtr = std::weak_ptr<T>;
#pragma endregion 

#pragma region Maps
	template<typename Key, typename Val>
	using hashMap = std::unordered_map<Key, Val>;
#pragma endregion 
}