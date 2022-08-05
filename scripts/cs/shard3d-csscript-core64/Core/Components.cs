using Shard3D.Core;
using System;

namespace Shard3D.Core
{
    public abstract class Component
    {
        public Actor _Actor { get; internal set; }
    }

}
namespace Shard3D.Components
{
    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(_Actor.ID, out Vector3 _translation);
                return _translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(_Actor.ID, ref value);
            }
        }
        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(_Actor.ID, out Vector3 _rotation);
                return _rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(_Actor.ID, ref value);
            }
        }
        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(_Actor.ID, out Vector3 _scale);
                return _scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(_Actor.ID, ref value);
            }
        }
    }
    public class PointlightComponent : Component
    {
        public Vector3 Color
        {
            get
            {
                InternalCalls.PointlightComponent_GetColor(_Actor.ID, out Vector3 _color);
                return _color;
            }
            set
            {
                InternalCalls.PointlightComponent_SetColor(_Actor.ID, ref value);
            }
        }
    }
}