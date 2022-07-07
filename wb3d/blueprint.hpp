#pragma once
#include "../s3dtpch.h"
#include "../components.hpp"
#include "actor.hpp"

namespace Shard3D {
	namespace wb3d {
		class Blueprint {

		public:
			Blueprint() = default;
			Blueprint(Actor actor, std::string path, std::string name);

			Blueprint(const Blueprint& other) = default;

			void attach(Actor actor) { actor.addComponent<Components::BlueprintComponent>().blueprint = this; }

			GUID getGUID() { return ID; }
			std::string getAssetFile() { return assetFile; }

			bool operator==(const Blueprint& other) const {
				return container == other.container;
			}
			bool operator!=(const Blueprint& other) const {
				return !(*this == other);
			}

		private:
			Actor container;

			GUID ID;
			std::string name = "Some kind of blueprint";
			std::string assetFile;

			friend class Level;
			friend class BlueprintManager;
		};
	}
}