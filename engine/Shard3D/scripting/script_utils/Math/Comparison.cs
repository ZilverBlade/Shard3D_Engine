namespace Shard3D.Core
{
    public static partial class Math
    {
        public static bool IsBetween(float value, float min, float max)
        {
            return (value < max) && (value > min);
        }
        public static bool IsBetweenOrEqual(float value, float min, float max)
        {
            return (value <= max) && (value >= min);
        }

        public static float Clamp(float value, float min, float max)
        {
            return System.Convert.ToSingle(value < min) * min + System.Convert.ToSingle(value > max) * max;
        }
    }

    
}