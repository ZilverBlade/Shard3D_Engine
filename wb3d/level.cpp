#include "level.hpp"
#include "actor.hpp"
#include "../components.hpp"


namespace Shard3D {
	namespace wb3d {
		Level::Level() {
			std::cout << "Creating level\n";
		}
		Level::~Level() {
			std::cout << "Destroying level\n";
			eRegistry.clear();
		}
		
		Actor Level::createActor(std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>();
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			return actor;
		}

		Actor Level::createActorWithGUID(GUID guid, std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::GUIDComponent>(guid);
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			return actor;
		}
		/*template<typename T>
		Actor Level::createActorWithComponent(std::string name) {
			assert(this != nullptr && "Level does not exist! Cannot create actors!");
			Actor actor = { eRegistry.create(), this };
			actor.addComponent<Components::TagComponent>().tag = name;
#ifdef ACTOR_FORCE_TRANSFORM_COMPONENT
			actor.addComponent<Components::TransformComponent>();
#endif
			actor.addComponent<T>();
			return actor;
		}
		*/
		void Level::runGarbageCollector(VkDevice device) {


			for (int i = 0; i < actorQueue.size(); i++) {
				std::cout << actorQueue.size() << "\n";
				vkDeviceWaitIdle(device);		
				eRegistry.destroy(actorQueue.at(i));

				actorQueue.pop_back();
			}
		}

		void Level::killActor(Actor actor) {
			actorQueue.emplace_back(actor);
		}

		void Level::killEverything() {

			eRegistry.each([&](auto actorGUID) { wb3d::Actor actor = { actorGUID, this };
				if (!actor) return;
				if (!actor.hasComponent<Components::TagComponent>()) return;
				if (actor.getGUID() == 0 || actor.getGUID() == std::numeric_limits<uint64_t>::max()) return;
				std::cout << actor.getTag() << "\n";
				
				killActor(actor);
			});
			//Log log;
			//log.logString("Destroying Level");
			//eRegistry.clear();
		}
		/*

		void Level::componentAdded() { }
		*/
	}
}