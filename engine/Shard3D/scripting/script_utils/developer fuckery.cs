using Shard3D.Core;
namespace Shard3D
{
    class DeveloperSandbox
    {
        void Test()
        {
            Vector3 scale = new Vector3(1.0F, 5.0F, 5.0F);
            Vector2 vecthing = new Vector2(4, 2);
            Vector4 vec4 = new Vector4(0, 0, 0, 5);
            Vector4 newvec = (vec4 + scale) * vecthing * 50.0F;
            bool issame = newvec == vec4;
            Vector4 lmao = new Vector4(vecthing, vecthing);
            Vector4 lmao2 = (Vector4)vecthing;
            Vector3 xd = new Vector2(4); // implicit casts go brrr

            Animation.TweenFloat tween;
            tween = new Animation.TweenLinearFloat(23, 4, 16);
        }
    }
}
