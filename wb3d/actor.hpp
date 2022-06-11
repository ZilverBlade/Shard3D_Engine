#pragma once

#ifdef NDEBUG
#define ENTT_ASSERT(...) ((void)1)
#endif
#include <entt.hpp>
#include "level.hpp"
#include "../engine_logger.hpp"
#include "../components.hpp"
#include "../scripts/script_handler.hpp"

namespace Shard3D {
	namespace wb3d {
		class Actor {

		public:
			Actor() = default;
			Actor(entt::entity handle, Level* level);

			Actor(const Actor& other) = default;

			template<typename T, typename... Args>
			T& addComponent(Args&&... args) {
				assert(!hasComponent<T>() && "Actor already has component!");
				//Log log;
				//log.logString("Added component ", true, false);
				//log.logString("name.component", false, true);

				std::cout << "Added component " << typeid(T).name() << "\n";
				return eLevel->registry.emplace<T>(actorHandle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& getComponent() {
#ifndef NDEBUG
				assert(hasComponent<T>() && "Actor does not have component!");
#endif
				return eLevel->registry.get<T>(actorHandle);
			}

			template<typename T>
			bool hasComponent() {
				return eLevel->registry.all_of<T>(actorHandle);
			}

			template<typename T>
			void killComponent() {
				assert(hasComponent<T>() && "Actor does not have component!");
				eLevel->registry.remove<T>(actorHandle);
			}

			operator bool() const { return actorHandle != entt::null; }
			operator entt::entity() const { return actorHandle; };
			operator uint32_t() const { return (uint32_t)actorHandle; };

			GUID getGUID() { return getComponent<Components::GUIDComponent>().id; }

			std::string getTag() { return getComponent<Components::TagComponent>().tag; }

			void setTag(std::string tag) { getComponent<Components::TagComponent>().tag = tag; };

			bool operator==(const Actor& other) const {
				return actorHandle == other.actorHandle && eLevel == other.eLevel;
			}
			bool operator!=(const Actor& other) const {
				return !(*this == other);
			}

		private:
			entt::entity actorHandle{entt::null};

			Level *eLevel = nullptr; // 12 bytes (use it as much as needed)

		};
	}
}