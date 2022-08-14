#pragma once
#include <cstdint>
namespace Shard3D {
	class UUID {
	public:
		UUID();
		UUID(uint64_t overrideGUID);
		UUID(const UUID&) = default;
		~UUID();

		operator uint64_t() const { return uuid; }
	private:
		uint64_t uuid;
	};
}

namespace std {
	template <typename T> struct hash;
	template<>
	struct hash<Shard3D::UUID> {
		size_t operator()(const Shard3D::UUID& uuid)const {
			return (uint64_t)uuid;
		}
	};
}