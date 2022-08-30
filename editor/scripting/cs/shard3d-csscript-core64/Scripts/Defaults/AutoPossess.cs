using Shard3D.Core;

namespace Shard3D.Scripts
{
	public class AutoPossessCamera : Actor
	{
		protected void BeginEvent()
		{
			if (this.HasComponent<Components.CameraComponent>())
            {
				this.GetComponent<Components.CameraComponent>().Possess();
            }
		}

		// Called when level stops playing
		protected void EndEvent()
		{			
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
