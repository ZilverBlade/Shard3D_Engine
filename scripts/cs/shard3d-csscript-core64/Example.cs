using Shard3D.Core;
using static Shard3D.Core.InternalCalls;

namespace Shard3D.Scripts
{
	public class Example : Actor
	{
		private ulong frames;
		private float elapsed;
		Vector3 clr;

		// Called when level starts playing
		protected void BeginEvent() {
			Log("BeginEvent() in C#", LogSeverity.Info);
			Log($"ID is {this.ID}", LogSeverity.Info);

			Log($"Do I have a PointlightComponent? {this.HasComponent<Components.PointlightComponent>()}!");
			Log($"Do I have a TransformComponent? {this.HasComponent<Components.TransformComponent>()}!");
			clr = this.AddComponent<Components.PointlightComponent>().Color;
			clr.y = 0.0F;
			this.GetComponent<Components.PointlightComponent>().Color = clr;
			this.GetComponent<Components.TransformComponent>().Translation = new Vector3(0.0F, 0.0F, 1.0F);
		}

		// Called when level stops playing
		protected void EndEvent() {

			Log("EndEvent() in C#", LogSeverity.Info);
		}

		// Called every frame, includes frametime value to keep game loop timing accurate
		protected void TickEvent(float dt) { // float dt = frametime 
			frames++;
			elapsed += (float)frames * dt * 0.005F;

			this.GetComponent<Components.PointlightComponent>().Color = new Vector3((float)System.Math.Sin(elapsed + 0.5F), (float)System.Math.Sin(elapsed + 0.0F), (float)System.Math.Sin(elapsed + 0.25F));
		}

		// Called on destruction of the actor
		protected void KillEvent()
		{
			Log("I died D:", LogSeverity.Info);
			Log($"I lasted {frames} frames");
		}

		// This is unused as there are no blueprints that can be spawned in with scripts
		protected void SpawnEvent() { }
	}
}
