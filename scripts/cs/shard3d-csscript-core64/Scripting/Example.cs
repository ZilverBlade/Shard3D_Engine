using Shard3D.Core;
using static Shard3D.Core.InternalCalls;

namespace Shard3D.Scripts
{
	public static class shit {
		public static ulong exampleID;
	}
	
	public class Example : Actor
	{
		private ulong frames;
		// Called when level starts playing
		protected void BeginEvent() {
			Log("BeginEvent() in C#", LogSeverity.Info);
			Log($"ID is {this.ID}", LogSeverity.Info);
			shit.exampleID = this.ID;
		}

		// Called when level stops playing
		protected void EndEvent() {
			Log("EndEvent() in C#", LogSeverity.Info);
		}

		// Called every frame, includes frametime value to keep game loop timing accurate
		protected void TickEvent(float dt) { // float dt = frametime 
			Vector3 _T = this.Translation;
			_T.y -= dt;
			this.Translation = _T;
			frames++;
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

	public class Destroyer : Actor
	{
		private ulong frames;
		private float elapsedTime;

		protected void BeginEvent()
		{
		
		}

		protected void EndEvent()
		{
			
		}

		protected void TickEvent(float dt)
		{ 
			frames++;
			elapsedTime = frames * dt;

			if (elapsedTime > 5)
            {
				ECS.KillActor(shit.exampleID);
				ECS.KillActor(this.ID);
			}

		}

		protected void KillEvent()
		{
			Log($"my job here is done", LogSeverity.Info);	
		}

		protected void SpawnEvent() { }
	}
}
