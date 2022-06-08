#pragma once
#include <vector>
#include "../utils/definitions.hpp"
#include <vulkan/vulkan.h>
#include <string>
#include "level.hpp"
#include "../device.hpp"
#include "levelmgr.hpp"

namespace Shard3D {
	namespace wb3d {
		class MasterManager {
		public:
			static void loadLevel(std::string path, EngineDevice& device);
			static void excecuteQueue(std::shared_ptr<Level>& level);
		private:
			inline static std::string levelPath;
			inline static EngineDevice* engineDevice;
		};
	}
}