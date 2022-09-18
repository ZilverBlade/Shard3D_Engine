namespace Shard3D.Core
{
    public static partial class Animation
    {
        public abstract class Tween
        {
            protected float alpha = 0.0f;
            protected float duration;

            public abstract bool Update(float deltaTime);
            public bool UpdateReverse(float deltaTime)
            {
                return Update(-deltaTime);
            }
            public virtual void Reset()
            {
                alpha = 0.0f;
            }
            public void Jump(float newTimePos)
            {
                duration = newTimePos;
            }
            public float GetAlpha()
            {
                return alpha;
            }
            public float GetElapsed()
            {
                return alpha * duration;
            }
        }

        public abstract partial class TweenFloat : Tween
        {
            protected float origin;
            protected float dest;
            protected float value;

            public float GetValue()
            {
                return value;
            }

            public override void Reset()
            {
                value = 0;
            }

            internal TweenFloat(float durationSeconds, float begin, float final)
            {
                duration = durationSeconds;
                origin = begin;
                dest = final; 
                value = begin;
            }
        }

        public abstract partial class TweenFloat2 : Tween
        {
            protected Vector2 origin;
            protected Vector2 dest;
            protected Vector2 value;

            public Vector2 GetValue()
            {
                return value;
            }

            public override void Reset()
            {
                value = new Vector2();
            }

            internal TweenFloat2(float durationSeconds, Vector2 begin, Vector2 final)
            {
                duration = durationSeconds;
                origin = begin;
                dest = final; 
                value = begin;
            }
        }

        public abstract partial class TweenFloat3 : Tween
        {
            protected Vector3 origin;
            protected Vector3 dest;
            protected Vector3 value;

            public Vector3 GetValue()
            {
                return value;
            }

            public override void Reset()
            {
                value = new Vector3();
            }

            internal TweenFloat3(float durationSeconds, Vector3 begin, Vector3 final)
            {
                duration = durationSeconds;
                origin = begin;
                dest = final; 
                value = begin;
            }
        }

        public abstract partial class TweenFloat4 : Tween
        {
            protected Vector4 origin;
            protected Vector4 dest;
            protected Vector4 value;
                            
            public Vector4 GetValue()
            {
                return value;
            }

            public override void Reset()
            {
                value = new Vector4();
            }

            internal TweenFloat4(float durationSeconds, Vector4 begin, Vector4 final)
            {
                duration = durationSeconds;
                origin = begin;
                dest = final;         
                value = begin;
            }
        }

        public class TweenLinearFloat : TweenFloat
        {
            public TweenLinearFloat(float durationSeconds, float begin, float final) : base(durationSeconds, begin, final)
            {
            }

            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {
                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }
               
                value = (1 - alpha) * origin + alpha * dest;

                return false;
            }
        }
        public class TweenLinearFloat2 : TweenFloat2
        {
            public TweenLinearFloat2(float durationSeconds, Vector2 begin, Vector2 final) : base(durationSeconds, begin, final)
            {
            }

            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {
                   
                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                value.x = (1 - alpha) * origin.x + alpha * dest.x;
                value.y = (1 - alpha) * origin.y + alpha * dest.y;

                return false;
            }
        }

        public class TweenLinearFloat3 : TweenFloat3
        {
            public TweenLinearFloat3(float durationSeconds, Vector3 begin, Vector3 final) : base(durationSeconds, begin, final)
            {
            }
            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {
                   
                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                value.x = (1 - alpha) * origin.x + alpha * dest.x;
                value.y = (1 - alpha) * origin.y + alpha * dest.y;
                value.z = (1 - alpha) * origin.z + alpha * dest.z;

                return false;
            }
        }

        public class TweenLinearFloat4 : TweenFloat4
        {
            public TweenLinearFloat4(float durationSeconds, Vector4 begin, Vector4 final) : base(durationSeconds, begin, final)
            {
            }
            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {
                   
                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                value.x = (1 - alpha) * origin.x + alpha * dest.x;
                value.y = (1 - alpha) * origin.y + alpha * dest.y;
                value.z = (1 - alpha) * origin.z + alpha * dest.z;
                value.w = (1 - alpha) * origin.w + alpha * dest.w;

                return false;
            }
        }

        public class TweenEaseFloat : TweenFloat
        {
            private float pow;
            private float destMinOrigin;

            public TweenEaseFloat(float durationSeconds, float begin, float final, float power = 2.0F) : base(durationSeconds, begin, final)
            {       
                destMinOrigin = final - begin;
                pow = power;
            }

            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {
                   
                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                float sqt = alpha * alpha;
                value = origin + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin;

                return false;
            }
        }
        public class TweenEaseFloat2 : TweenFloat2
        {
            private float pow;
            private Vector2 destMinOrigin;

            public TweenEaseFloat2(float durationSeconds, Vector2 begin, Vector2 final, float power = 2.0F) : base(durationSeconds, begin, final)
            {
                destMinOrigin = final - begin;
                pow = power;
            }

            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {

                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                float sqt = alpha * alpha;
                value.x = origin.x + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.x;
                value.y = origin.y + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.y;
                
                return false;
            }
        }
        public class TweenEaseFloat3 : TweenFloat3
        {
            private float pow;
            private Vector3 destMinOrigin;
           
            public TweenEaseFloat3(float durationSeconds, Vector3 begin, Vector3 final, float power = 2.0F) : base(durationSeconds, begin, final)
            {
                duration = durationSeconds;
                origin = begin;
                dest = final; value = begin;

                destMinOrigin = final - begin;
                pow = power;
            }

            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {
                   
                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                float sqt = alpha * alpha;
                value.x = origin.x + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.x;
                value.y = origin.y + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.y;
                value.z = origin.z + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.z;

                return false;
            }
        }

        public class TweenEaseFloat4 : TweenFloat4
        {
            private float pow;
            private Vector4 destMinOrigin;

            public TweenEaseFloat4(float durationSeconds, Vector4 begin, Vector4 final, float power = 2.0F) : base(durationSeconds, begin, final)
            {
                destMinOrigin = final - begin;
                pow = power;
            }

            // Updates the interpolation, returns true when the tween has ended
            public override bool Update(float deltaTime)
            {
                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {

                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                float sqt = alpha * alpha;
                value.x = origin.x + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.x;
                value.y = origin.y + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.y;
                value.z = origin.z + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.z;
                value.w = origin.w + sqt / (2.0F * (sqt - alpha) + 1.0f) * destMinOrigin.w;

                return false;
            }
        }

        public class TweenBezierFloat : TweenFloat
        {
            private float destMinOrigin;
            private Math.Bezier curve;

            public float p1x = 0;
            public float p1y = 0;
             
            public float p2x = 0;
            public float p2y = 1;
             
            public float p3x = 1;
            public float p3y = 0;
             
            public float p4x = 1;
            public float p4y = 1;

            public TweenBezierFloat(float durationSeconds, float begin, float final, Math.Bezier curve_) : base(durationSeconds, begin, final)
            {
                destMinOrigin = final - begin;
                curve = curve_;
            }

            public override bool Update(float deltaTime)
            {

                alpha += deltaTime / duration;
                if (!Math.IsBetween(alpha, 0f, 1f))
                {

                    alpha = Math.Clamp(alpha, 0, 1f);
                    return true;
                }

                double xfunc = (p1x * (System.Math.Pow(1 - alpha, 3))) + (3 * p2x * alpha * System.Math.Pow(1 - alpha, 2)) + (3 * p3x * System.Math.Pow(alpha, 2) * (1 - alpha)) + (System.Math.Pow(alpha, 3) * p4x);
                double yfunc = (p1y * (System.Math.Pow(1 - alpha, 3))) + (3 * p2y * alpha * System.Math.Pow(1 - alpha, 2)) + (3 * p3y * System.Math.Pow(alpha, 2) * (1 - alpha)) + (System.Math.Pow(alpha, 3) * p4y);
                alpha += 0.01f;
                double hyp = System.Math.Sqrt(System.Math.Pow(xfunc, 2) + System.Math.Pow(yfunc, 2));
                double resultant = System.Math.Sqrt(System.Math.Pow(xfunc, 2) + System.Math.Pow(yfunc, 2) + (2 * xfunc * yfunc * (xfunc / hyp)));
      
                value = (float)(origin + resultant * destMinOrigin);

                return false;
            }
        }
    }
}
