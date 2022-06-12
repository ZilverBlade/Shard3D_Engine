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
			static void captureLevel(std::shared_ptr<Level>& level);
			static void executeQueue(std::shared_ptr<Level>& level);     
		protected:
			inline static std::shared_ptr<Level> lc;
		public:
			inline static std::shared_ptr<Level>& levelCapture = lc;
		private:
			inline static std::string levelPath = "wb3d_nullpath";;
			inline static EngineDevice* engineDevice;
		};
	}
}