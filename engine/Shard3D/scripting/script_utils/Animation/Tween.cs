namespace Shard3D.Core
{
    public static partial class Animation
    {
        public abstract class Tween
        {
            protected float elapsedAlpha = 0.0f;
            protected float duration;

            public abstract bool Update(float deltaTime);
            public bool UpdateReverse(float deltaTime)
            {
                return Update(-deltaTime);
            }
            public virtual void Reset()
            {
                elapsedAlpha = 0.0f;
            }
            public void Jump(float newTimePos)
            {
                duration = newTimePos;
            }
            public float GetAlpha()
            {
                return elapsedAlpha;
            }
            public float GetElapsed()
            {
                return elapsedAlpha * duration;
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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {
                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }
               
                value = (1 - elapsedAlpha) * origin + elapsedAlpha * dest;

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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {
                   
                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }

                value.x = (1 - elapsedAlpha) * origin.x + elapsedAlpha * dest.x;
                value.y = (1 - elapsedAlpha) * origin.y + elapsedAlpha * dest.y;

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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {
                   
                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }

                value.x = (1 - elapsedAlpha) * origin.x + elapsedAlpha * dest.x;
                value.y = (1 - elapsedAlpha) * origin.y + elapsedAlpha * dest.y;
                value.z = (1 - elapsedAlpha) * origin.z + elapsedAlpha * dest.z;

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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {
                   
                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }

                value.x = (1 - elapsedAlpha) * origin.x + elapsedAlpha * dest.x;
                value.y = (1 - elapsedAlpha) * origin.y + elapsedAlpha * dest.y;
                value.z = (1 - elapsedAlpha) * origin.z + elapsedAlpha * dest.z;
                value.w = (1 - elapsedAlpha) * origin.w + elapsedAlpha * dest.w;

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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {
                   
                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }

                float sqt = elapsedAlpha * elapsedAlpha;
                value = origin + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin;

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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {

                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }

                float sqt = elapsedAlpha * elapsedAlpha;
                value.x = origin.x + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.x;
                value.y = origin.y + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.y;
                
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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {
                   
                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }

                float sqt = elapsedAlpha * elapsedAlpha;
                value.x = origin.x + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.x;
                value.y = origin.y + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.y;
                value.z = origin.z + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.z;

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
                elapsedAlpha += deltaTime / duration;
                if (!Math.IsBetween(elapsedAlpha, 0f, 1f))
                {

                    elapsedAlpha = Math.Clamp(elapsedAlpha, 0, 1f);
                    return true;
                }

                float sqt = elapsedAlpha * elapsedAlpha;
                value.x = origin.x + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.x;
                value.y = origin.y + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.y;
                value.z = origin.z + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.z;
                value.w = origin.w + sqt / (2.0F * (sqt - elapsedAlpha) + 1.0f) * destMinOrigin.w;

                return false;
            }
        }
    }
}
