using Shard3D.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Shard3D.Scripts
{
	public class CarDriver : Actor
	{
		private const float basePitch = 0.1f;
		private Components.TransformComponent transform;
		private Components.AudioComponent audio;
		private float exponent;
		private Vector3 moveVector;

		private ulong exhaustActorID = 2106594163601404339;

		private ulong rearwheelhubID = 8660709294248625364;
		private ulong frontleftwheelID = 13339742369598458913;
		private ulong frontrightwheelID = 3256940297627109905;
		private Components.TransformComponent frontleftwheelIDtransform;
		private Components.TransformComponent frontrightwheelIDtransform;
		private Components.TransformComponent rearwheelhubIDtransform;

		protected void BeginEvent()
		{
			transform = this.GetComponent<Components.TransformComponent>();
			audio = ECS.GetActor(exhaustActorID).GetComponent<Components.AudioComponent>();
			audio.Play();

			frontleftwheelIDtransform = ECS.GetActor(frontleftwheelID).GetComponent<Components.TransformComponent>();
			frontrightwheelIDtransform = ECS.GetActor(frontrightwheelID).GetComponent<Components.TransformComponent>();
			rearwheelhubIDtransform = ECS.GetActor(rearwheelhubID).GetComponent<Components.TransformComponent>();
		}

		protected void EndEvent()
		{
			audio.Stop();
		}

		protected void TickEvent(float dt)
		{
			var rotation = transform.Rotation;
			var translation = transform.Translation;
			var audioProperties = audio.Properties;

			var rotationWFR = frontrightwheelIDtransform.Rotation;
			
			float yaw = rotation.z;
			Vector3 forwardDir = new Vector3((float)Math.Sin(yaw), (float)Math.Cos(yaw), 0.0f);
			
			if (Input.IsKeyDown(KeyInput.KEY_D))
			{
				rotationWFR.z += 0.4f * dt;
				rotationWFR.z = Math.Min(rotationWFR.z, 0.5f);
			}
			if (Input.IsKeyDown(KeyInput.KEY_A))
			{
				rotationWFR.z -= 0.4f * dt;
				rotationWFR.z = Math.Max(rotationWFR.z, -0.5f);
			}
			if (Input.IsKeyDown(KeyInput.KEY_S))
				moveVector -= dt;
			if (Input.IsKeyDown(KeyInput.KEY_W))
			{
				exponent += dt * 0.05f; 
				moveVector += exponent * dt * 0.05f;
				rotation.z += rotationWFR.z * dt;
				audioProperties.volume = 0.5f;
			}
			else
			{
				moveVector.x -= dt * 0.0098f;
				moveVector.y -= dt * 0.0098f;
				exponent -= 0.05f * dt;
				if (exponent < 0) exponent = 0.0001f;
				if (moveVector.x < 0) moveVector.x = 0.0f;
				if (moveVector.y < 0) moveVector.y = 0.0f;
				audioProperties.volume = 0.2f;
			}

			rotationWFR.x += moveVector.y;

			frontrightwheelIDtransform.Rotation = rotationWFR;
			frontleftwheelIDtransform.Rotation = rotationWFR;
			rearwheelhubIDtransform.Rotation = new Vector3(rotationWFR.x, 0.0f, 0.0f);

			translation += moveVector * forwardDir;

			audioProperties.relativePos = translation;
			audioProperties.pitch = basePitch + moveVector.y * 3.0f;


			audio.Properties = audioProperties;
			audio.Update();
			transform.Rotation = rotation;
			transform.Translation = translation;
		}

		protected void KillEvent()
		{

		}

		protected void SpawnEvent() { }
	}
}
