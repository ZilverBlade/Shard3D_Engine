using Shard3D.Core;

namespace Shard3D.Scripts
{
	public class TrollUI : Actor
	{
		protected void BeginEvent()
		{ 
			SceneManager.LoadHUDTemplate("assets/huddata/trolliumbg.wbht", 0);
			SceneManager.LoadHUDTemplate("assets/huddata/trollium menu.wbht", 1);
		}

		// Called when level stops playing
		protected void EndEvent()
		{
			SceneManager.DestructHUDLayer(0);
			SceneManager.DestructHUDLayer(1);
		}

		// Called every frame, includes frametime value to keep game loop timing accurate
		protected void TickEvent(float dt)
		{
		}

		// Called on destruction of the actor
		protected void KillEvent()
		{
		}

		// This is unused as there are no blueprints that can be spawned in with scripts
		protected void SpawnEvent() { }
	}
}
