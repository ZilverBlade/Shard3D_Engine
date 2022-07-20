#pragma once
#include "../frame_info.hpp"	  
#include "../components.hpp"

namespace Shard3D {
	class PointlightSystem {
	public:
		PointlightSystem(const PointlightSystem&) = delete;
		PointlightSystem& operator=(const PointlightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level);
	};

}