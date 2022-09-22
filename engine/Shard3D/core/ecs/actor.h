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
#ifndef ENSET_UNSAFE_COMPONENTS
				if (hasComponent<T>()) {	// return getComponent if component exists
					SHARD3D_ERROR("Actor {0} already has component {1}!", this->getUUID(), typeid(T).name());
					return getComponent<T>();
					//SHARD3D_FATAL("Tried to add component when component already is present!");
				}
#endif
				return level->registry.emplace<T>(actorHandle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& getComponent() const {
#ifndef ENSET_UNSAFE_COMPONENTS
				if (!hasComponent<T>()) { // is error since it will very likely cause a crash
					SHARD3D_ERROR("Actor {0} does not have component '{1}'!", this->getUUID(), typeid(T).name());
					SHARD3D_FATAL("Tried to get a component that does not exist!");
				}
#endif
				return level->registry.get<T>(actorHandle);
			}

			template<typename T>
			bool hasComponent() const {
				return level->registry.all_of<T>(actorHandle);
			}

			template<typename T>
			void killComponent() const { // is warn since it has the chance of not causing a crash or undefined behaviour
				if (!hasComponent<T>()) { // no #if !ENSET_UNSAFE_COMPONENTS because this error is negligible;
					SHARD3D_WARN("Actor {0} does not have component '{1}'!", this->getUUID(), typeid(T).name());
					return;
				}
				level->registry.remove<T>(actorHandle);
			}

			inline bool hasRelationship() const {
				return hasComponent<Components::RelationshipComponent>();
			}

			bool isInvalid() const {
				return actorHandle == entt::null 
					|| (getUUID() == 0 || getUUID() == 1 || getUUID() == UINT64_MAX)
					|| !hasComponent<Components::TagComponent>()
					|| !hasComponent<Components::UUIDComponent>();
			}
			inline UUID getUUID() const { return getComponent<Components::UUIDComponent>().getID(); }
			inline std::string getTag() const { return getComponent<Components::TagComponent>().tag; }
			inline void setTag(std::string tag) const { getComponent<Components::TagComponent>().tag = tag; };
			inline Components::TransformComponent& getTransform() const { return getComponent<Components::TransformComponent>(); }

			operator bool() const { return actorHandle != entt::null; }
			operator entt::entity() const { return actorHandle; };
			operator uint32_t() const { return static_cast<uint32_t>(actorHandle); };
			
			bool operator==(const Actor& other) const {
				return actorHandle == other.actorHandle && level == other.level;
			}
			bool operator!=(const Actor& other) const {
				return !(*this == other);
			}
			entt::entity actorHandle{ entt::null };
		private:
			Level* level = nullptr; // 8 bytes (use it as much as needed)

			friend class Level;
			friend class SActor;
			friend class LevelPeekingPanel;
		};
	}
}