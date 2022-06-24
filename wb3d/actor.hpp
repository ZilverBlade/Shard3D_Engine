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
				if (hasComponent<T>()) {	// is error, not because it's likely to cause a crash, but because idk how to handle the return
					SHARD3D_ERROR("Actor {0} already has component {1}!", this->getGUID(), typeid(T).name());
					throw std::runtime_error("Incorrect component!");
				}
				SHARD3D_LOG("Added component {0}", typeid(T).name());
				return eLevel->registry.emplace<T>(actorHandle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& getComponent() {
				if (!hasComponent<T>()) { // is error since it will very likely cause a crash
					SHARD3D_ERROR("Actor {0} does not have component '{1}'!", this->getGUID(), typeid(T).name());
					throw std::runtime_error("Incorrect component!");
				}
				return eLevel->registry.get<T>(actorHandle);
			}

			template<typename T>
			bool hasComponent() {
				return eLevel->registry.all_of<T>(actorHandle);
			}

			template<typename T>
			void killComponent() { // is warn since it has the chance of not causing a crash or undefined behaviour
				if (!hasComponent<T>()) {
					SHARD3D_WARN("Actor {0} does not have component '{1}'!", this->getGUID(), typeid(T).name());
					return;
				}
				eLevel->registry.remove<T>(actorHandle);
			}
			
			GUID getGUID() { return getComponent<Components::GUIDComponent>().id; }
			std::string getTag() { return getComponent<Components::TagComponent>().tag; }
			void setTag(std::string tag) { getComponent<Components::TagComponent>().tag = tag; };

			operator bool() const { return actorHandle != entt::null; }
			operator entt::entity() const { return actorHandle; };
			operator uint32_t() const { return (uint32_t)actorHandle; };
			
			bool operator==(const Actor& other) const {
				return actorHandle == other.actorHandle && eLevel == other.eLevel;
			}
			bool operator!=(const Actor& other) const {
				return !(*this == other);
			}

		private:
			entt::entity actorHandle{entt::null};

			Level *eLevel = nullptr; // 8 bytes (use it as much as needed)

			friend class Level;
			friend class ActingActor;
			friend class BlueprintManager;
		};
	}
}