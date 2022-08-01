#pragma once
#include <entt.hpp>
#include "../GUID.hpp"
#include "../utils/definitions.hpp"
#include "../camera.hpp"
#include "../device.hpp"
namespace Shard3D {
	class EditorApp;
	namespace wb3d {
		class Actor;
		class Blueprint;
		class LevelTreePanel;
		enum class PlayState {
			Stopped = 0, // no scripts have been instantiated. outside of the editor this should only be this state when loading levels.
			Playing = 2, // unused, for future when stuff like player controls can be properly integrated in game.
			Simulating = 1, // indicates that the level is being simulated in the editor.
			Paused = -1, // regular pause mode, for the editor, where all input pauses including GUI, and repossesses editor camera.
			PausedRuntime = -2	// special pause state where everything pauses except UI input, for game runtime like pause menu etc.
		};

		class Level {
		public:

			Level(const std::string& lvlName = "Some kind of level");
			~Level();

			static std::shared_ptr<Level> copy(std::shared_ptr<Level> other);

			Blueprint createBlueprint(Actor actor, std::string path, std::string name = "Some kind of blueprint");

			Actor createActor(const std::string& name= "Some kind of actor");
			
			void killEverything();
			void killActor(Actor actor);
			void killMesh(Actor actor);
			void killTexture(Actor actor);
			void reloadMesh(Actor actor);
			void reloadTexture(Actor actor);

			void runGarbageCollector(EngineDevice& device);

			Actor getActorFromGUID(GUID guid);
			// Unreliable function as multiple actors can have identical tags, use getActorFromGUID() whenever possible.
			Actor getActorFromTag(const std::string& tag);

			void setPossessedCameraActor(Actor actor);
			void setPossessedCameraActor(GUID guid);
			Actor getPossessedCameraActor();
			EngineCamera& getPossessedCamera();

			void parentActor(Actor parent, Actor child);

			/* *
* Call when level events must begin
*/
			void begin();
			/* *
* Gets called upon every frame
* \param dt Frame time
*/
			void tick(float dt);
			/* *
* Call when level events must end
*/
			void end();

			void simulationStateCallback();

			PlayState simulationState = PlayState::Stopped;

			bool wasPaused;

			entt::registry registry;
			std::string name = "Some kind of level";
			std::string currentpath;
		protected:
			GUID possessedCameraActorGUID;
			GUID _possessedCameraActorGUID;
#if ENSET_ALLOW_PREVIEW_CAMERA // ONLY FOR DEBUGGING PURPOSES
			GUID possessedPreviewCameraActorGUID;
#endif  
		private:
			//should only be called by system processes
			Actor createActorWithGUID(GUID guid, const std::string& name = "Some kind of actor");
#if ENSET_ALLOW_PREVIEW_CAMERA // ONLY FOR DEBUGGING PURPOSES
			void setPossessedPreviewCameraActor(Actor actor);
			void setPossessedPreviewCameraActor(GUID guid);
			Actor getPossessedPreviewCameraActor();
			EngineCamera& getPossessedPreviewCamera();
#endif


			bool loadRegistryCapture = false;

			// map
			std::unordered_map<GUID, entt::entity> actorMap;

			// queues 
			std::vector<Actor> actorKillQueue;
			std::vector<Actor> actorKillTexQueue;
			std::vector<Actor> actorReloadTexQueue;
			std::vector<Actor> actorKillMeshQueue;
			std::vector<Actor> actorReloadMeshQueue;

			friend class Shard3D::EditorApp;
			friend class Actor;
			friend class MasterManager;

			friend class LevelManager;
			friend class LevelTreePanel;
			friend class LevelPropertiesPanel;
			friend class LevelPeekingPanel;
		};
	}
}