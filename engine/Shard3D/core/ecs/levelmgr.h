#pragma once

#include "level.h"
#include "../../dynamic_definitions.h"

namespace Shard3D {
	namespace ECS {
		enum class LevelMgrResults {
			SuccessResult,
			FailedResult,
			UnknownResult,
			ErrorResult,

			WrongFileResult,
			InvalidEntryResult,
			OldEngineVersionResult,
			FutureVersionResult,
		};

		class LevelManager {
		public:
			LevelManager(sPtr<Level>& level);

			void save(const std::string& destinationPath, bool encryptLevel = false);
			void saveRuntime(const std::string& destinationPath);

			LevelMgrResults load(const std::string& sourcePath, bool ignoreWarns = false);
			LevelMgrResults loadRuntime(const std::string& sourcePath);

		private:
			sPtr<Level>& mLevel;
		};
	}
}