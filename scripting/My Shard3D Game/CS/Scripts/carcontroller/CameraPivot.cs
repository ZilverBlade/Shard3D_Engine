using Shard3D.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Shard3D.Scripts
{
	public class CameraPivot : Actor
	{
		Components.TransformComponent transform;

		protected void BeginEvent()
		{
			transform = this.GetComponent<Components.TransformComponent>();
		}

		protected void EndEvent()
		{

		}

		protected void TickEvent(float dt)
		{
			if (Input.IsKeyDown(KeyInput.KEY_J))
			{
				var rot = transform.Rotation;
				rot.z += 3.0F * dt;
				transform.Rotation = rot;
			}

			if (Input.IsKeyDown(KeyInput.KEY_L))
			{
				var rot = transform.Rotation;
				rot.z -= 3.0F * dt;
				transform.Rotation = rot;
			}
		}

		protected void KillEvent()
		{

		}

		protected void SpawnEvent() { }
	}
}
