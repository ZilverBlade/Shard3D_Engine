#pragma once
#include "../frame_info.hpp"	  
#include "../components.hpp"

namespace Shard3D {
	class SpotlightSystem {
	public:
		SpotlightSystem(const SpotlightSystem&) = delete;
		SpotlightSystem& operator=(const SpotlightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level);
	};
}