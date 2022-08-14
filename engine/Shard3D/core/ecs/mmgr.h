#pragma once

#include "../../core.h"
#include "../vulkan_api/device.h"
#include "level.h"
#include "levelmgr.h"

namespace Shard3D {
	namespace ECS {
		class MasterManager {
		public:
			static void loadLevel(std::string path);
			static void captureLevel(sPtr<Level>& level);
			static void executeQueue(sPtr<Level>& level, EngineDevice& engineDevice);
		private:
			static inline std::string levelPath = "ECS_nullpath";
			static inline sPtr<Level> capturedLevel{};
		};
	}
}