using Shard3D.Core;
using static Shard3D.Core.InternalCalls;
using Shard3D.Core.Components;

namespace Shard3D.Scripts
{
	public class Example : Actor
	{ 
		Example() {
				Log($"Le id {this.ID}", LogSeverity.Info);
		}
		protected void BeginEvent() {
			Log("BeginEvent() in C#", LogSeverity.Info);
			//Log(frames.ToString());
			Log($"ID is {this.ID}", LogSeverity.Info);
		}

		protected void EndEvent() {
			Log("EndEvent() in C#", LogSeverity.Info);
		}

		protected void TickEvent(float dt) { // float dt = frametime 

		}

		protected void SpawnEvent() {
		
		}

		protected void KillEvent() {
		
		}
	}
}
