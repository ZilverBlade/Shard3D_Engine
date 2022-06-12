#include "master_manager.hpp"
#include <iostream>

namespace Shard3D {
	namespace wb3d {
		void MasterManager::loadLevel(std::string path, EngineDevice& device) {
			engineDevice = &device;
			levelPath = path;
		}
		class Actor;
		void MasterManager::captureLevel(std::shared_ptr<Level>& level) {
			std::cout << "capturing level\n";
			//memcpy_s(&regCap, sizeof(regCap), &level->registry, sizeof(level->registry));

			//std::cout << regCap.alive() << "\n";
		}
		void MasterManager::executeQueue(std::shared_ptr<Level>& level) {
			if (levelPath != "wb3d_nullpath") {
				//level = std::make_shared<Level>();
				LevelManager levelMan(level);
				LevelMgrResults result = levelMan.load(levelPath, *engineDevice, false);
				if (result == LevelMgrResults::OldEngineVersionResult) {
					if (MessageBoxA(NULL, "This level was created in an different version of Shard3D, results may be unexpected.\nWould you like to try and load the level anyway?", "WARNING!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
						levelMan.load(levelPath, *engineDevice, true);
					}
					else {
						//std::cout << "you fucked up :)\n";
					}
				}
				levelPath = "wb3d_nullpath";
			}
			if (level->loadRegistryCapture) {
				std::cout << "loading back level capture\n";
				/*
				const std::shared_ptr<Level>& lvl = std::make_shared<Level>();
				memmove(&lvl->registry, &regCap, sizeof(regCap));
				regCap = {};
				level = std::make_shared<Level>();
				level = lvl;

				std::cout << level->registry.alive() << "\n";
*/
				//level = level;
				level->loadRegistryCapture = false;
			}
		}
	}
}