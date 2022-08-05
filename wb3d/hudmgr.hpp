#pragma once

#include "../device.hpp"
#include "../hud.hpp"
#include <memory>
namespace Shard3D {
	namespace wb3d {
		enum class HUDMgrResults {
			SuccessResult = 0,
			FailedResult = 1,
			UnknownResult = -1,
			ErrorResult = -2,

			WrongFileResult = 10,
			InvalidEntryResult = 20,
			OldEngineVersionResult = 30,
			OldEditorVersionResult = 31,
			FutureVersionResult = 40,
			FutureEditorVersionResult = 41
		};

		class HUDManager {
		public:
			HUDManager(HUDContainer* container);

			std::string encrypt(std::string input);
			std::string decrypt(std::string input);

			void save(const std::string& destinationPath, int layer, bool encryptGui = false);
			void saveRuntime(const std::string& destinationPath);

			HUDMgrResults load(const std::string& sourcePath, int layer, bool ignoreWarns = false);
			HUDMgrResults loadRuntime(const std::string& sourcePath);

		private:
			HUDContainer* mHud;
		};
	}
}