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
			//InternalCalls.Log("BeginEvent() in C#", LogSeverity.Info);
			//InternalCalls.Log($"ID is {this.ID}", LogSeverity.Info);
			//		 
			//InternalCalls.Log($"Do I have a PointlightComponent? {this.HasComponent<Components.PointlightComponent>()}!");
			//InternalCalls.Log($"Do I have a TransformComponent? {this.HasComponent<Components.TransformComponent>()}!");
		}

		// Called when level stops playing
		protected void EndEvent()
		{
			//InternalCalls.Log("EndEvent() in C#", LogSeverity.Info);
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
			//InternalCalls.Log("I died D:", LogSeverity.Info);
			//InternalCalls.Log($"I lasted {frames} frames, aka I lasted {elapsed / 1000.0F} seconds");
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

	public class Tweener : Actor
	{
		public float time = 2.5f;
		private Shard3D.Core.Animation.TweenLinearFloat tween;
		bool keydown= false;
		bool reverse = false;
		protected void BeginEvent()
		{
			tween = new Core.Animation.TweenLinearFloat(time, 0, 10);
		}

		protected void EndEvent()
		{
		}

		protected void TickEvent(float dt)
		{
			Vector3 Translation = this.GetComponent<Components.TransformComponent>().Translation;
		
			bool isDone;
			if (reverse)
				isDone = tween.UpdateReverse(dt);
			else
				isDone = tween.Update(dt);

			if (isDone != true)
            {
				Translation.z = tween.GetValue();
            }
			
			if (Input.IsKeyDown(KeyInput.KEY_SPACE) && keydown == false)
            {
				keydown = true;
				if (reverse)
					reverse = false;
				else reverse = true;
				System.Console.WriteLine("Reversing");
			}
            else
            {
				keydown = false;
			}

			System.Console.WriteLine("alpha is " + tween.GetAlpha());
			this.GetComponent<Components.TransformComponent>().Translation = Translation;
		}

		protected void KillEvent()
		{
		}

		protected void SpawnEvent()
		{
		}
	}

	public class CameraAnimTest : Actor
	{
		private Shard3D.Core.Animation.TweenFloat3 tweenTransl;
		private Shard3D.Core.Animation.TweenFloat3 tweenRot;

		private Vector3 beginTransl;
		private Vector3 beginRot;

		protected void BeginEvent()
		{
			this.GetComponent<Components.CameraComponent>().Possess();

			beginTransl = this.GetComponent<Components.TransformComponent>().Translation;
			beginRot = this.GetComponent<Components.TransformComponent>().Rotation;

			tweenTransl = new Core.Animation.TweenEaseFloat3(1, beginTransl, new Vector3(-1.45f, -3.06f, 0.412f));
			tweenRot = new Core.Animation.TweenEaseFloat3(1, beginRot, new Vector3(0f, 0f, 0.3f));
		}

		protected void EndEvent()
		{
		}

		protected void TickEvent(float dt)
		{
			if (!Input.IsKeyDown(KeyInput.KEY_LEFT_SHIFT))
			{
				if (Input.IsKeyDown(KeyInput.KEY_SPACE))
				{
					tweenTransl.Update(dt);
					tweenRot.Update(dt);
				}
				else
				{
					tweenTransl.UpdateReverse(dt);
					tweenRot.UpdateReverse(dt);
				}
			}

			if (Input.IsKeyDown(KeyInput.KEY_Q))
			{
				tweenTransl = new Core.Animation.TweenEaseFloat3(1, beginTransl, new Vector3(1.45f, -2.06f, 1.512f));
				tweenRot = new Core.Animation.TweenEaseFloat3(1, beginRot, new Vector3(0.5f, 0.2f, -0.7f));
			}
			if (Input.IsKeyDown(KeyInput.KEY_W))
			{
				tweenTransl = new Core.Animation.TweenEaseFloat3(1, beginTransl, new Vector3(-1.45f, -3.06f, 0.412f));
				tweenRot = new Core.Animation.TweenEaseFloat3(1, beginRot, new Vector3(0f, 0f, -0.3f));
			}
			if (Input.IsKeyDown(KeyInput.KEY_A))
			{
				tweenTransl = new Core.Animation.TweenLinearFloat3(1, beginTransl, new Vector3(1.45f, -2.06f, 1.512f));
				tweenRot = new Core.Animation.TweenLinearFloat3(1, beginRot, new Vector3(0.5f, 0.2f, -0.7f));
			}
			if (Input.IsKeyDown(KeyInput.KEY_S))
			{
				tweenTransl = new Core.Animation.TweenLinearFloat3(1, beginTransl, new Vector3(-1.45f, -3.06f, 0.412f));
				tweenRot = new Core.Animation.TweenLinearFloat3(1, beginRot, new Vector3(0f, 0f, -0.3f));
			}
			this.GetComponent<Components.TransformComponent>().Translation = tweenTransl.GetValue();
			this.GetComponent<Components.TransformComponent>().Rotation = tweenRot.GetValue();
		}

		protected void KillEvent()
		{
		}

		protected void SpawnEvent()
		{
		}
	}
}
