#include "master_manager.hpp"


namespace Shard3D {
	namespace wb3d {
		void MasterManager::loadLevel(std::string path, EngineDevice& device) {
			engineDevice = &device;
			levelPath = path;
		}
		void MasterManager::excecuteQueue(std::shared_ptr<Level>& level) {
			if (levelPath != "wb3d_nullpath") {
				//level = std::make_shared<Level>();
				LevelManager levelMan(level);
				levelMan.load(levelPath, *engineDevice, false);
				levelPath = "wb3d_nullpath";
			}
		}
	}
}