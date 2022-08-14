#pragma once

#include "../../core/ecs/components.h"
#include "actor.h"

namespace Shard3D {
	namespace ECS {
		class Blueprint {

		public:
			Blueprint() = default;
			Blueprint(Actor actor, std::string path, std::string name);

			Blueprint(const Blueprint& other) = default;

			void attach(Actor actor) { actor.addComponent<Components::BlueprintComponent>().blueprint = this; }

			UUID getUUID() { return ID; }
			std::string getAssetFile() { return assetFile; }

			bool operator==(const Blueprint& other) const {
				return container == other.container;
			}
			bool operator!=(const Blueprint& other) const {
				return !(*this == other);
			}

		private:
			Actor container;

			UUID ID;
			std::string name = "Some kind of blueprint";
			std::string assetFile;

			friend class Level;
			friend class BlueprintManager;
		};
	}
}