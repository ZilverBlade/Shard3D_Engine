#pragma once
#ifdef NDEBUG
#define ENTT_ASSERT(...) ((void)1)
#endif
#include <entt.hpp>
#include "level.h"
#include "components.h"
#include "../../core.h"
#include "../../scripting/script_handler.h"

namespace Shard3D {
	namespace ECS {
		class Actor {
		public:
			Actor() = default;
			Actor(entt::entity _handle, Level* _level);

			Actor(const Actor& other) = default;

			template<typename T, typename... Args>
			T& addComponent(Args&&... args) {
#ifndef ENSET_CONFIDENT_COMPONENTS
				if (hasComponent<T>()) {	// return getComponent if component exists
					SHARD3D_ERROR("Actor {0} already has component {1}!", this->getUUID(), typeid(T).name());
					return getComponent<T>();
					//SHARD3D_FATAL("Tried to add component when component already is present!");
				}
#endif
				return level->registry.emplace<T>(actorHandle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& getComponent() {
#ifndef ENSET_CONFIDENT_COMPONENTS
				if (!hasComponent<T>()) { // is error since it will very likely cause a crash
					SHARD3D_ERROR("Actor {0} does not have component '{1}'!", this->getUUID(), typeid(T).name());
					SHARD3D_FATAL("Tried to get a component that does not exist!");
				}
#endif
				return level->registry.get<T>(actorHandle);
			}

			template<typename T>
			bool hasComponent() {
				return level->registry.all_of<T>(actorHandle);
			}

			template<typename T>
			void killComponent() { // is warn since it has the chance of not causing a crash or undefined behaviour
				if (!hasComponent<T>()) { // no #if !ENSET_CONFIDENT_COMPONENTS because this error is negligible;
					SHARD3D_WARN("Actor {0} does not have component '{1}'!", this->getUUID(), typeid(T).name());
					return;
				}
				level->registry.remove<T>(actorHandle);
			}

			bool isInvalid() {
				return actorHandle == entt::null 
					|| (getUUID() == 0 || getUUID() == 1 || getUUID() == UINT64_MAX)
					|| !hasComponent<Components::TagComponent>()
					|| !hasComponent<Components::UUIDComponent>();
			}
			inline UUID getUUID() { return getComponent<Components::UUIDComponent>().id; }
			inline std::string getTag() { return getComponent<Components::TagComponent>().tag; }
			inline void setTag(std::string tag) { getComponent<Components::TagComponent>().tag = tag; };
			inline Components::TransformComponent& getTransform() { return getComponent<Components::TransformComponent>(); }

			operator bool() const { return actorHandle != entt::null; }
			operator entt::entity() const { return actorHandle; };
			operator uint32_t() const { return (uint32_t)actorHandle; };
			
			bool operator==(const Actor& other) const {
				return actorHandle == other.actorHandle && level == other.level;
			}
			bool operator!=(const Actor& other) const {
				return !(*this == other);
			}
			entt::entity actorHandle{ entt::null };
			entt::entity parentHandle{ entt::null };
		private:
			Level* level = nullptr; // 8 bytes (use it as much as needed)

			friend class Level;
			friend class SActor;
			friend class BlueprintManager;
			friend class LevelPeekingPanel;
		};
	}
}