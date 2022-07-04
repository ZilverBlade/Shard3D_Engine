#pragma once
#include "../device.hpp"
#include "actor.hpp"
#include "blueprint.hpp"

namespace Shard3D {
	namespace wb3d {
		enum class BlueprintMgrResults {
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

		class BlueprintManager {
		public:
			BlueprintManager(Actor& mActor);

			std::string encrypt(std::string input);
			std::string decrypt(std::string input);

			void removeAllComponents(Actor actor);

			void convert(const std::string& destinationPath, Blueprint blueprint, bool encryptLevel = false);
			void convertRuntime(const std::string& destinationPath);

			BlueprintMgrResults load(const std::string& sourcePath, bool ignoreWarns = false);
			BlueprintMgrResults loadRuntime(const std::string& sourcePath);
			
		private:
			Actor& actor;
		};
	}
}