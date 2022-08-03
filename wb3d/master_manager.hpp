#pragma once

#include "../utils/definitions.hpp"

#include "level.hpp"
#include "../device.hpp"
#include "levelmgr.hpp"

namespace Shard3D {
	namespace wb3d {
		class MasterManager {
		public:
			static void loadLevel(std::string path);
			static void captureLevel(std::shared_ptr<Level>& level);
			static void executeQueue(std::shared_ptr<Level>& level, EngineDevice& engineDevice);
		private:
			_S3D_GVAR std::string levelPath = "wb3d_nullpath";;
			_S3D_GVAR EngineDevice* engineDevice;
		};
	}
}