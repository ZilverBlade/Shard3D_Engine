using Shard3D.Core;

namespace Shard3D.Scripts
{
	public class Example : Actor
	{
		private ulong frames;
		private float elapsed;

		// Called when level starts playing
		protected void BeginEvent()
		{
			InternalCalls.Log("BeginEvent() in C#", LogSeverity.Info);
			InternalCalls.Log($"ID is {this.ID}", LogSeverity.Info);
						 
			InternalCalls.Log($"Do I have a PointlightComponent? {this.HasComponent<Components.PointlightComponent>()}!");
			InternalCalls.Log($"Do I have a TransformComponent? {this.HasComponent<Components.TransformComponent>()}!");
		}

		// Called when level stops playing
		protected void EndEvent()
		{
			InternalCalls.Log("EndEvent() in C#", LogSeverity.Info);
		}

		// Called every frame, includes frametime value to keep game loop timing accurate
		protected void TickEvent(float dt)
		{ // float dt = frametime 
			frames++;

			elapsed += dt;
		}

		// Called on destruction of the actor
		protected void KillEvent()
		{
			InternalCalls.Log("I died D:", LogSeverity.Info);
			InternalCalls.Log($"I lasted {frames} frames, aka I lasted {elapsed / 1000.0F} seconds");
		}

		// This is unused as there are no blueprints that can be spawned in with scripts
		protected void SpawnEvent() { }
	}

    public class ZRotate : Actor
    {
        protected void BeginEvent()
        {
        }

        protected void EndEvent()
        {
        }

        protected void TickEvent(float dt)
        {
            var rotation = this.GetComponent<Components.TransformComponent>().Rotation;
			rotation.z += dt * 10;
			this.GetComponent<Components.TransformComponent>().Rotation = rotation;
        }

        protected void KillEvent()
        {
        }

        protected void SpawnEvent()
        {
        }
    }
}
