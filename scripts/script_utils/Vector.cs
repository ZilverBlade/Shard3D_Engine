namespace Shard3D.Core
{
    public struct Vector2
    {
        public float x;
        public float y;
        public Vector2(float xVal, float yVal)
        {
            x = xVal;
            y = yVal;
        }
        public Vector2(float uVal)
        {
            x = uVal;
            y = uVal;
        }
        public static implicit operator Vector2(float[] value)
        {
            return new Vector2(value[0], value[1]);
        }

        public static Vector2 operator +(Vector2 first, Vector2 second)
        {
            return new Vector2(first.x + second.x, first.y + second.y);
        }
        public static Vector2 operator -(Vector2 first, Vector2 second)
        {
            return new Vector2(first.x - second.x, first.y - second.y);
        }
        public static Vector2 operator *(Vector2 first, Vector2 second)
        {
            return new Vector2(first.x * second.x, first.y * second.y);
        }
        public static Vector2 operator /(Vector2 first, Vector2 second)
        {
            return new Vector2(first.x / second.x, first.y / second.y);
        }

        public static Vector2 operator *(Vector2 first, float second)
        {
            return new Vector2(first.x * second, first.y * second);
        }
        public static Vector2 operator /(Vector2 first, float second)
        {
            return new Vector2(first.x / second, first.y / second);
        }
        // No sum operator as you'd not know what axis to sum to
        public static bool operator ==(Vector2 first, Vector2 second)
        {
            return first.x == second.x && first.y == second.y;
        }
        public static bool operator !=(Vector2 first, Vector2 second)
        {
            return !(first == second);
        }
    }
    public struct Vector3
    {
        public float x;
        public float y;
        public float z;
        public Vector3(float xVal, float yVal, float zVal)
        {
            x = xVal;
            y = yVal;
            z = zVal;
        }
        public Vector3(Vector2 vec2, float zVal)
        {
            x = vec2.x;
            y = vec2.y;
            z = zVal;
        }  
        public Vector3(float uVal)
        {
            x = uVal;
            y = uVal;
            z = uVal;         
        }
        public static implicit operator Vector3(float[] value)
        {
            return new Vector3(value[0], value[1], value[2]);
        }

        public static Vector3 operator +(Vector3 first, Vector3 second)
        {
            return new Vector3(first.x + second.x, first.y + second.y, first.z + second.z);
        }
        public static Vector3 operator -(Vector3 first, Vector3 second)
        {
            return new Vector3(first.x - second.x, first.y - second.y, first.z - second.z);
        }
        public static Vector3 operator *(Vector3 first, Vector3 second)
        {
            return new Vector3(first.x * second.x, first.y * second.y, first.z * second.z);
        }
        public static Vector3 operator /(Vector3 first, Vector3 second)
        {
            return new Vector3(first.x / second.x, first.y / second.y, first.z / second.z);
        }

        public static Vector3 operator +(Vector3 first, Vector2 second)
        {
            return new Vector3(first.x + second.x, first.y + second.y, first.z);
        }
        public static Vector3 operator -(Vector3 first, Vector2 second)
        {
            return new Vector3(first.x - second.x, first.y - second.y, first.z);
        }
        public static Vector3 operator *(Vector3 first, Vector2 second)
        {
            return new Vector3(first.x * second.x, first.y * second.y, first.z);
        }
        public static Vector3 operator /(Vector3 first, Vector2 second)
        {
            return new Vector3(first.x / second.x, first.y / second.y, first.z);
        }
        public static Vector3 operator *(Vector3 first, float second)
        {
            return new Vector3(first.x * second, first.y * second, first.z * second);
        }
        public static Vector3 operator /(Vector3 first, float second)
        {
            return new Vector3(first.x / second, first.y / second, first.z / second);
        }
        // No sum operator as you'd not know what axis to sum to

        public static implicit operator Vector3(Vector2 value)
        {
            return new Vector3(value.x, value.y, 0.0F);
        }
        public static bool operator ==(Vector3 first, Vector3 second)
        {
            return first.x == second.x && first.y == second.y && first.z == second.z;
        }
        public static bool operator !=(Vector3 first, Vector3 second)
        {
            return !(first == second);
        }
    }
    public struct Vector4
    {
        public float x;
        public float y;
        public float z;
        public float w;
        public Vector4(float xVal, float yVal, float zVal, float wVal)
        {
            x = xVal;
            y = yVal;
            z = zVal;
            w = wVal;
        }
        public Vector4(Vector2 vec2x, Vector2 vec2y)
        {
            x = vec2x.x;
            y = vec2x.y;
            z = vec2y.x;
            w = vec2y.y;
        }
        public Vector4(Vector2 vec2, float zVal, float wVal)
        {
            x = vec2.x;
            y = vec2.y;
            z = zVal;
            w = wVal;
        }
        public Vector4(Vector3 vec3, float wVal)
        {
            x = vec3.x;
            y = vec3.y;
            z = vec3.z;
            w = wVal;
        }
        public Vector4(float uVal)
        {
            x = uVal;
            y = uVal;
            z = uVal;
            w = uVal;
        }
        public static implicit operator Vector4(float[] value)
        {
            return new Vector4(value[0], value[1], value[2], value[3]);
        }

        public static Vector4 operator +(Vector4 first, Vector4 second)
        {
            return new Vector4(first.x + second.x, first.y + second.y, first.z + second.z, first.w + second.w);
        }
        public static Vector4 operator -(Vector4 first, Vector4 second)
        {
            return new Vector4(first.x - second.x, first.y - second.y, first.z - second.z, first.w - second.w);
        }
        public static Vector4 operator *(Vector4 first, Vector4 second)
        {
            return new Vector4(first.x * second.x, first.y * second.y, first.z * second.z, first.w * second.w);
        }
        public static Vector4 operator /(Vector4 first, Vector4 second)
        {
            return new Vector4(first.x / second.x, first.y / second.y, first.z / second.z, first.w / second.w);
        }

        public static Vector4 operator +(Vector4 first, Vector3 second)
        {
            return new Vector4(first.x + second.x, first.y + second.y, first.z + second.z, first.w);
        }
        public static Vector4 operator -(Vector4 first, Vector3 second)
        {
            return new Vector4(first.x - second.x, first.y - second.y, first.z - second.z, first.w);
        }
        public static Vector4 operator *(Vector4 first, Vector3 second)
        {
            return new Vector4(first.x * second.x, first.y * second.y, first.z * second.z, first.w);
        }
        public static Vector4 operator /(Vector4 first, Vector3 second)
        {
            return new Vector4(first.x / second.x, first.y / second.y, first.z / second.z, first.w);
        }

        public static Vector4 operator +(Vector4 first, Vector2 second)
        {
            return new Vector4(first.x + second.x, first.y + second.y, first.z, first.w);
        }
        public static Vector4 operator -(Vector4 first, Vector2 second)
        {
            return new Vector4(first.x - second.x, first.y - second.y, first.z, first.w);
        }
        public static Vector4 operator *(Vector4 first, Vector2 second)
        {
            return new Vector4(first.x * second.x, first.y * second.y, first.z, first.w);
        }
        public static Vector4 operator /(Vector4 first, Vector2 second)
        {
            return new Vector4(first.x / second.x, first.y / second.y, first.z, first.w);
        }
        public static Vector4 operator *(Vector4 first, float second)
        {
            return new Vector4(first.x * second, first.y * second, first.z * second, first.w * second);
        }
        public static Vector4 operator /(Vector4 first, float second)
        {
            return new Vector4(first.x / second, first.y / second, first.z / second, first.w / second);
        }
        // No sum operator as you'd not know what axis to sum to


        public static implicit operator Vector4(Vector2 value)
        {
            return new Vector4(value.x, value.y, 0.0F, 0.0F);
        }

        public static implicit operator Vector4(Vector3 value)
        {
            return new Vector4(value.x, value.y, value.z, 0.0F);
        }

        public static bool operator ==(Vector4 first, Vector4 second)
        {
            return first.x == second.x && first.y == second.y && first.z == second.z && first.w == second.w;
        }
        public static bool operator !=(Vector4 first, Vector4 second)
        {
            return !(first == second);
        }
    }
}
