#pragma once
#include "../s3dtpch.h"
#include "../device.hpp"
#include "level.hpp"
namespace Shard3D {
	namespace wb3d {
		enum class LevelMgrResults {
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

		class LevelManager {
		public:
			LevelManager(const std::shared_ptr<Level>& level);

			std::string encrypt(std::string input);
			std::string decrypt(std::string input);

			void save(const std::string& destinationPath, bool encryptLevel = false);
			void saveRuntime(const std::string& destinationPath);

			LevelMgrResults load(const std::string& sourcePath, bool ignoreWarns = false);
			LevelMgrResults loadRuntime(const std::string& sourcePath);

		private:
			std::shared_ptr<Level> mLevel;
		};
	}
}