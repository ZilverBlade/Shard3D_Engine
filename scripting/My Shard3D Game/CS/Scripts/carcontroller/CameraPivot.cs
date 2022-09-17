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
		Core.Animation.TweenEaseFloat cameraTweenJ;
		Core.Animation.TweenEaseFloat cameraTweenL;
		Core.Animation.TweenEaseFloat cameraTweenK;

		Components.TransformComponent transform;

		bool firstTapJ = true, firstTapL = true, firstTapK = true;

		protected void BeginEvent()
		{
			transform = this.GetComponent<Components.TransformComponent>();
			cameraTweenJ = new Core.Animation.TweenEaseFloat(0.5F, 0, 1.5F);
			cameraTweenL = new Core.Animation.TweenEaseFloat(0.5F, 0, -1.5F);
			cameraTweenK = new Core.Animation.TweenEaseFloat(1.0F, 0, 3.14159F);
		}

		protected void EndEvent()
		{

		}

		protected void TickEvent(float dt)
		{
			if (Input.IsKeyDown(KeyInput.KEY_J))
			{
				if (firstTapJ)
                {
					firstTapJ = false;
				}

				if (!cameraTweenJ.Update(dt))
                {
					var rot = transform.Rotation;
					rot.z = cameraTweenJ.GetValue();
					transform.Rotation = rot;
				}
			}
			else if (!firstTapJ)
			{
				firstTapJ = true;
			}
			else if (firstTapJ && (cameraTweenJ.GetAlpha() > 0))
			{
				cameraTweenJ.UpdateReverse(dt);
				var rot = transform.Rotation;
				rot.z = cameraTweenJ.GetValue();
				transform.Rotation = rot;
			}

			if (Input.IsKeyDown(KeyInput.KEY_L))
			{
				if (firstTapL)
				{
					firstTapL = false;
				}

				if (!cameraTweenL.Update(dt))
				{
					var rot = transform.Rotation;
					rot.z = cameraTweenL.GetValue();
					transform.Rotation = rot;
				}
			}
			else if (!firstTapL)
			{
				firstTapL = true;
			}
			else if (firstTapL && (cameraTweenL.GetAlpha() > 0))
			{
				cameraTweenL.UpdateReverse(dt);
				var rot = transform.Rotation;
				rot.z = cameraTweenL.GetValue();
				transform.Rotation = rot;
			}

			if (Input.IsKeyDown(KeyInput.KEY_K))
			{
				if (firstTapK)
				{
					firstTapK = false;
				}

				if (!cameraTweenK.Update(dt))
				{
					var rot = transform.Rotation;
					rot.z = cameraTweenK.GetValue();
					transform.Rotation = rot;
				}
			}
			else if (!firstTapK)
			{
				firstTapK = true;
			}
			else if (firstTapK && (cameraTweenK.GetAlpha() > 0))
			{
				cameraTweenK.UpdateReverse(dt);
				var rot = transform.Rotation;
				rot.z = cameraTweenK.GetValue();
				transform.Rotation = rot;
			}
		}

		protected void KillEvent()
		{

		}

		protected void SpawnEvent() { }
	}
}
