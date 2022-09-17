namespace Shard3D.Core
{
    public static partial class Math
    {
        public static class Lerp
        {
            public static float Float(float alpha, float begin, float end)
            {
                return (1 - alpha) * begin + alpha * end;
            }
            public static Vector2 Float2(float alpha, Vector2 begin, Vector2 end)
            {
                Vector2 result;
                result.x = (1 - alpha) * begin.x + alpha * end.x;
                result.y = (1 - alpha) * begin.y + alpha * end.y;
                return result;
            }
            public static Vector3 Float3(float alpha, Vector3 begin, Vector3 end)
            {
                Vector3 result;
                result.x = (1 - alpha) * begin.x + alpha * end.x;
                result.y = (1 - alpha) * begin.y + alpha * end.y;
                result.z = (1 - alpha) * begin.z + alpha * end.z;
                return result;
            }
            public static Vector4 Float4(float alpha, Vector4 begin, Vector4 end)
            {
                Vector4 result;
                result.x = (1 - alpha) * begin.x + alpha * end.x;
                result.y = (1 - alpha) * begin.y + alpha * end.y;
                result.z = (1 - alpha) * begin.z + alpha * end.z;
                result.w = (1 - alpha) * begin.w + alpha * end.w;
                return result;
            }
        }
    }
}
