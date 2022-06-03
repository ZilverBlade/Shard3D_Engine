#pragma once

#include "level.hpp"
namespace Shard3D {
	namespace wb3d {
		enum class LevelMgrResults {
			SuccessResult = 0,
			FailedResult = 1,
			UnknownResult = -1,
			ErrorResult = -2,

			WrongFileResult = 10,
			InvalidEntryResult = 20
		};

		class LevelManager {
		public:
			LevelManager(const std::shared_ptr<Level>& level);

			void save(const std::string& destinationPath);
			void saveRuntime(const std::string& destinationPath);

			LevelMgrResults load(const std::string& sourcePath);
			LevelMgrResults loadRuntime(const std::string& sourcePath);

		private:
			std::shared_ptr<Level> mLevel;
		};
	}
}