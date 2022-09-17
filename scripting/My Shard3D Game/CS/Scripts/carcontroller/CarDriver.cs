using Shard3D.Core;
namespace Shard3D.Scripts
{
	public class CarDriver : Actor
	{
		public string onFile = "assets/audiodata/race_engine_3_on.wav";
		public string offFile = "assets/audiodata/race_engine_3_off.wav";

		public float max_steering_velocity_rad_per_sec = 0.8f;

		private Animation.TweenFloat steeringTween;

		private const float basePitch = 0.1f;
		private Components.TransformComponent transform;

		private Components.AudioComponent audioOn;
		private Components.AudioComponent audioOff;
		
		private float exponent;
		private Vector3 moveVector;

		private ulong exhaustOnActorID = 11921769603700759874;
		private ulong exhaustOffActorID = 12116567100235884741;

		private ulong rearwheelhubID = 8660709294248625364;
		private ulong frontleftwheelID = 13339742369598458913;
		private ulong frontrightwheelID = 3256940297627109905;
		private Components.TransformComponent frontleftwheelIDtransform;
		private Components.TransformComponent frontrightwheelIDtransform;
		private Components.TransformComponent rearwheelhubIDtransform;

		protected void BeginEvent()
		{
			transform = this.GetComponent<Components.TransformComponent>();
			audioOn = ECS.GetActor(exhaustOnActorID).GetComponent<Components.AudioComponent>();
			audioOn.Play();
			audioOff = ECS.GetActor(exhaustOffActorID).GetComponent<Components.AudioComponent>();
			audioOff.Play();

			frontleftwheelIDtransform = ECS.GetActor(frontleftwheelID).GetComponent<Components.TransformComponent>();
			frontrightwheelIDtransform = ECS.GetActor(frontrightwheelID).GetComponent<Components.TransformComponent>();
			rearwheelhubIDtransform = ECS.GetActor(rearwheelhubID).GetComponent<Components.TransformComponent>();

			steeringTween = new Animation.TweenEaseFloat(0.7f, -0.5f, 0.5f);
			steeringTween.Jump(0.35f);
		}

		protected void EndEvent()
		{
			audioOn.Stop();
			audioOff.Stop();
		}

		protected void TickEvent(float dt)
		{
			var rotation = transform.Rotation;
			var translation = transform.Translation;
			var audioOnProperties = audioOn.Properties;
			var audioOffProperties = audioOff.Properties;

			var rotationWFR = frontrightwheelIDtransform.Rotation;

			float yaw = rotation.z;
			Vector3 forwardDir = new Vector3((float)System.Math.Sin(yaw), (float)System.Math.Cos(yaw), 0.0f);
			
			if (Input.IsKeyDown(KeyInput.KEY_D))
			{			
				steeringTween.Update(dt);
			}
			else if(steeringTween.GetAlpha() >= 0.5)
            {
				steeringTween.UpdateReverse(dt);
			}
			if (Input.IsKeyDown(KeyInput.KEY_A))
			{
				steeringTween.UpdateReverse(dt);
			}
			else if (steeringTween.GetAlpha() < 0.5)
			{
				steeringTween.Update(dt);
			}
			if (Input.IsKeyDown(KeyInput.KEY_S))
				moveVector -= dt;
			if (Input.IsKeyDown(KeyInput.KEY_W))
			{
				exponent += dt * 0.05f; 
				moveVector += exponent * dt * 0.05f;
				audioOnProperties.volume = 0.7f;
				audioOffProperties.volume = 0.0f;
			}
			else
			{
				moveVector.x -= dt * 0.0178f;
				moveVector.y -= dt * 0.0178f;
				exponent -= 0.05f * dt;
				if (exponent < 0) exponent = 0.0001f;
				if (moveVector.x < 0) moveVector.x = 0.0f;
				if (moveVector.y < 0) moveVector.y = 0.0f;
				audioOnProperties.volume = 0.0f;
				audioOffProperties.volume = 0.5f;
			}
			float c = 90 * moveVector.y;
			rotation.z += rotationWFR.z * dt * Math.Clamp(c * c, 0.0f, max_steering_velocity_rad_per_sec);
			rotationWFR.x += moveVector.y;

			rotationWFR.z = steeringTween.GetValue();
			frontrightwheelIDtransform.Rotation = rotationWFR;
			frontleftwheelIDtransform.Rotation = rotationWFR;
			rearwheelhubIDtransform.Rotation = new Vector3(rotationWFR.x, 0.0f, 0.0f);

			translation += moveVector * forwardDir;

			audioOnProperties.relativePos = translation;
			audioOnProperties.pitch = basePitch + moveVector.y * 3.0f;

			audioOffProperties.relativePos = translation;
			audioOffProperties.pitch = basePitch + moveVector.y * 3.0f;

			audioOn.Properties = audioOnProperties;
			audioOff.Properties = audioOffProperties;
			audioOn.Update();
			audioOff.Update();
			transform.Rotation = rotation;
			transform.Translation = translation;
		}

		protected void KillEvent()
		{

		}

		protected void SpawnEvent() { }
	}
}
