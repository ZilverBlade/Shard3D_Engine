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
		class LevelPeekingPanel;
		class Actor {

		public:
			Actor() = default;
			Actor(entt::entity handle, Level* level);

			Actor(const Actor& other) = default;

			template<typename T, typename... Args>
			T& addComponent(Args&&... args) {
#if !ENSET_CONFIDENT_COMPONENTS
				if (hasComponent<T>()) {	// return getComponent if component exists
					SHARD3D_ERROR("Actor {0} already has component {1}!", this->getGUID(), typeid(T).name());
					return getComponent<T>();
					//SHARD3D_FATAL("Tried to add component when component already is present!");
				}
#endif
				return eLevel->registry.emplace<T>(actorHandle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& getComponent() {
#if !ENSET_CONFIDENT_COMPONENTS
				if (!hasComponent<T>()) { // is error since it will very likely cause a crash
					SHARD3D_ERROR("Actor {0} does not have component '{1}'!", this->getGUID(), typeid(T).name());
					SHARD3D_FATAL("Tried to get a component that does not exist!");
				}
#endif
				return eLevel->registry.get<T>(actorHandle);
			}

			template<typename T>
			bool hasComponent() {
				return eLevel->registry.all_of<T>(actorHandle);
			}

			template<typename T>
			void killComponent() { // is warn since it has the chance of not causing a crash or undefined behaviour
				if (!hasComponent<T>()) { // no #if !ENSET_CONFIDENT_COMPONENTS because this error is negligible;
					SHARD3D_WARN("Actor {0} does not have component '{1}'!", this->getGUID(), typeid(T).name());
					return;
				}
				eLevel->registry.remove<T>(actorHandle);
			}
			
			bool isInvalid() {
				return (getGUID() == 0 || getGUID() == 1 || getGUID() == UINT64_MAX)
					|| !hasComponent<Components::TagComponent>()
					|| actorHandle == entt::null;
			}
			GUID getGUID() { return getComponent<Components::GUIDComponent>().id; }
			std::string getTag() { return getComponent<Components::TagComponent>().tag; }
			void setTag(std::string tag) { getComponent<Components::TagComponent>().tag = tag; };

#if ENSET_ACTOR_FORCE_TRANSFORM_COMPONENT
			Components::TransformComponent& getTransform() { return getComponent<Components::TransformComponent>(); }
#endif

			operator bool() const { return actorHandle != entt::null; }
			operator entt::entity() const { return actorHandle; };
			operator uint32_t() const { return (uint32_t)actorHandle; };
			
			bool operator==(const Actor& other) const {
				return actorHandle == other.actorHandle && eLevel == other.eLevel;
			}
			bool operator!=(const Actor& other) const {
				return !(*this == other);
			}
			entt::entity actorHandle{ entt::null };
		private:

			Level *eLevel = nullptr; // 8 bytes (use it as much as needed)

			friend class Level;
			friend class ActingActor;
			friend class BlueprintManager;
			friend class LevelPeekingPanel;
		};
	}
}