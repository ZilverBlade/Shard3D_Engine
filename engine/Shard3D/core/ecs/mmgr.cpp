#include "../../s3dpch.h"
#include "mmgr.h"

namespace Shard3D {
	namespace ECS {
		void MasterManager::loadLevel(std::string path) {
			levelPath = path;
		}
		class Actor;
		void MasterManager::captureLevel(sPtr<Level>& level) {
			SHARD3D_INFO("Capturing level");
			capturedLevel = make_sPtr<Level>();
			capturedLevel = Level::copy(level);
		}
		void MasterManager::executeQueue(sPtr<Level>& level, EngineDevice& engineDevice) {
			if (levelPath != "ECS_nullpath") {
				LevelManager levelMan(level);
				LevelMgrResults result = levelMan.load(levelPath, false);
				if (result == LevelMgrResults::OldEngineVersionResult) {
					if (MessageDialogs::show("This level was created in an different version of Shard3D, results may be unexpected.\nWould you like to try and load the level anyway?", "WARNING!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
						levelMan.load(levelPath, true);
					}
					else {
						//std::cout << "you fucked up :)\n";
					}
				}
				levelPath = "ECS_nullpath";
			}
			if (level->loadRegistryCapture) {
				SHARD3D_INFO("Loading back Captured level");
				level = capturedLevel;
				capturedLevel = make_sPtr<Level>("__WB3D:NOEDITcapturelvl");
				level->loadRegistryCapture = false;
			}
		}
	}
}