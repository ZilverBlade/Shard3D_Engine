#pragma once


#include "actor.hpp"

namespace Shard3D {
	namespace wb3d {
		class ActingActor {
		public:
			template<typename T>
			T& getComponent() {
				return aActor.getComponent<T>();
			}
		private:
			Actor aActor;
			friend class Level;
		};
	}
}