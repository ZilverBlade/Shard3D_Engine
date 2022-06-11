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
			static void executeQueue(std::shared_ptr<Level>& level);

			inline static Level* levelCapture;
		private:
			//std::shared_ptr<Level>& levelPtrCapture = nullptr;

			inline static std::string levelPath = "wb3d_nullpath";;
			inline static EngineDevice* engineDevice;
		};
	}
}