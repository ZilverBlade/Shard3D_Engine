#include "master_manager.hpp"
#include <iostream>
#include "../engine_logger.hpp"

namespace Shard3D {
	namespace wb3d {
		void MasterManager::loadLevel(std::string path) {
			levelPath = path;
		}
		class Actor;
		void MasterManager::captureLevel(std::shared_ptr<Level>& level) {
			SHARD3D_INFO("Capturing level");

			levelCapture = level->copy(level);
		}
		void MasterManager::executeQueue(std::shared_ptr<Level>& level, EngineDevice& engineDevice) {
			if (levelPath != "wb3d_nullpath") {
				LevelManager levelMan(level);
				LevelMgrResults result = levelMan.load(levelPath, false);
				if (result == LevelMgrResults::OldEngineVersionResult) {
					if (MessageBoxA(NULL, "This level was created in an different version of Shard3D, results may be unexpected.\nWould you like to try and load the level anyway?", "WARNING!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
						levelMan.load(levelPath, true);
					}
					else {
						//std::cout << "you fucked up :)\n";
					}
				}
				levelPath = "wb3d_nullpath";
			}
			if (level->loadRegistryCapture) {
				SHARD3D_INFO("Loading back Captured level");

				level = level->copy(levelCapture);
				levelCapture = std::make_shared<Level>("__WB3D:NOEDITcapturelvl");

				level->loadRegistryCapture = false;
			}
		}
	}
}