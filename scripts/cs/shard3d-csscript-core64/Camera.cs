using Shard3D.Core;

namespace Shard3D.Scripts
{
    public class CameraRotator : Actor
    {
        private Components.CameraComponent camera;
        private Components.TransformComponent transform;
        private bool hasEnded;

        protected void BeginEvent()
        {
            camera = this.GetComponent<Components.CameraComponent>();
            camera.Possess();
            transform = this.GetComponent<Components.TransformComponent>();
            var t = transform.Translation;
            var r = transform.Rotation;
            t.z = 4.0F;
            r.x = 35.0F;

            transform.Translation = t;
            transform.Rotation = r;
        }

        protected void EndEvent()
        {
        }

        protected void TickEvent(float dt)
        {
            var translation = transform.Translation;
            var rotation = transform.Rotation;
            if (hasEnded == false)
            {
                var f = camera.FOV;
                f -= 2 * dt;
                camera.FOV = f;
                translation.z -= 0.2F * dt;
                rotation.x -= 1.5F * dt;
                if (transform.Translation.z < 1.3F)
                    hasEnded = true;
            }
            transform.Translation = translation;
            transform.Rotation = rotation;
        }

        protected void KillEvent()
        {
        }

        protected void SpawnEvent()
        {
        }
    }
}
