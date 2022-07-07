#pragma once
#include "s3dtpch.h"
#include <xhash>

namespace Shard3D {
	class GUID {
	public:
		GUID();
		GUID(uint64_t overrideGUID);
		GUID(const GUID&) = default;
		~GUID();

		operator uint64_t() const { return guid; }
	private:
		uint64_t guid;
	};
}

namespace std {
	template<>
	struct hash<Shard3D::GUID> {
		std::size_t operator()(const Shard3D::GUID& guid)const {
			return hash<uint64_t>()((uint64_t)guid);
		}
	};
}