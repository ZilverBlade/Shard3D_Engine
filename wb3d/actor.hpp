#pragma once
#include <entt.hpp>
#include "scene.hpp"
#include "../engine_logger.hpp"
namespace Shard3D {
	namespace wb3d {
		class Actor {

		public:
			Actor() = default;
			Actor(entt::entity handle, Scene* scene);
			~Actor();

			Actor(const Actor& other) = default;

			template<typename T, typename... Args>
			T& addComponent(Args&&... args) {
				assert(!hasComponent<T>() && "Actor already has component!");
				Log log;
				log.logString("Created actor ", true, false);
				log.logString("<actor name>", false, true);
				return eScene->eRegistry.emplace<T>(actorHandle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& getComponent() {
				assert(hasComponent(T) && "Actor does not have component!");
				return eScene->eRegistry.get<T>(actorHandle);
			}

			template<typename T>
			bool hasComponent() {
				return eScene->eRegistry.all_of<T>(actorHandle);
			}

			template<typename T>
			void removeComponent() {
				assert(hasComponent(T) && "Actor does not have component!");
				eScene->eRegistry.remove<T>(actorHandle);
			}

			operator bool() const { return actorHandle != entt::null; }
		private:
			entt::entity actorHandle{entt::null};
			Scene *eScene = nullptr; // 12 bytes (use it as much as needed)

		};
	}
}