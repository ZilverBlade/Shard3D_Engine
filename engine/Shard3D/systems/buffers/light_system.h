#pragma once
#include "../../core/misc/frame_info.h"	  
#include "../../core/ecs/level.h"

namespace Shard3D {
	class LightSystem {
	public:
		LightSystem() = default;
		LightSystem(const LightSystem&) = delete;
		LightSystem& operator=(const LightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
	};

}