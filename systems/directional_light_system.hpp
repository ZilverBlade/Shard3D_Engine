#pragma once
#include "../frame_info.hpp"	  
#include "../components.hpp"

namespace Shard3D {
	class DirectionalLightSystem {
	public:
		DirectionalLightSystem(const DirectionalLightSystem&) = delete;
		DirectionalLightSystem& operator=(const DirectionalLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo, std::shared_ptr<wb3d::Level>& level);
	};

}