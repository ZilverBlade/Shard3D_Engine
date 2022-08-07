using Shard3D.Core;

namespace Shard3D.Scripts
{
	public class UISpawner : Actor
	{

		protected void BeginEvent()
		{
			SceneManager.LoadHUDTemplate("assets/huddata/test.wbht", 1);
		}

		protected void EndEvent()
		{
			SceneManager.DestructHUDLayer(1);
		}

		protected void TickEvent(float dt)
		{

		}

		protected void KillEvent()
		{

		}

		protected void SpawnEvent() { }
	}
}
