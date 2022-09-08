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
    public class CameraComponent : Component
    {
        public Core.CameraProjectionType ProjectionType
        {
            get
            {
                InternalCalls.CameraComponent_GetProjectionType(_Actor.ID, out int _pt);
                return (CameraProjectionType)_pt;
            }
            set
            {
                int val = (int)value;
                InternalCalls.CameraComponent_SetProjectionType(_Actor.ID, ref val);
            }
        }
        public float FOV
        {
            get
            {
                InternalCalls.CameraComponent_GetFOV(_Actor.ID, out float _fov);
                return _fov;
            }
            set
            {
                InternalCalls.CameraComponent_SetFOV(_Actor.ID, ref value);
            }
        }
        public float NearClipPlane
        {
            get
            {
                InternalCalls.CameraComponent_GetNearClip(_Actor.ID, out float _nc);
                return _nc;
            }
            set
            {
                InternalCalls.CameraComponent_SetNearClip(_Actor.ID, ref value);
            }
        }

        public float FarClipPlane
        {
            get
            {
                InternalCalls.CameraComponent_GetFarClip(_Actor.ID, out float _fc);
                return _fc;
            }
            set
            {
                InternalCalls.CameraComponent_SetFarClip(_Actor.ID, ref value);
            }
        }

        public void Possess()
        {
            InternalCalls.SceneManager_PossessCameraActor(_Actor.ID);
        }
    }
    public class AudioComponent : Component
    {
        public string File
        {
            get
            {
                InternalCalls.AudioComponent_GetFile(_Actor.ID, out string _f);
                return _f;
            }
            set
            {
                InternalCalls.AudioComponent_SetFile(_Actor.ID, value);
            }
        }
        public Core.AudioProperties Properties
        {
            get
            {
                AudioProperties _ap = new AudioProperties();
                InternalCalls.AudioComponent_GetPropertiesVolume(_Actor.ID, out _ap.volume);
                InternalCalls.AudioComponent_GetPropertiesPitch(_Actor.ID, out _ap.pitch);
                return _ap;
            }
            set
            {
                InternalCalls.AudioComponent_SetPropertiesVolume(_Actor.ID, ref value.volume);
                InternalCalls.AudioComponent_SetPropertiesPitch(_Actor.ID, ref value.pitch);
            }
        }
        public void Play()
        {
            InternalCalls.AudioComponent_Play(_Actor.ID);
        }
        public void Stop()
        {
            InternalCalls.AudioComponent_Stop(_Actor.ID);
        }
        public void Pause()
        {
            InternalCalls.AudioComponent_Pause(_Actor.ID);
        }
        public void Resume()
        {
            InternalCalls.AudioComponent_Resume(_Actor.ID);
        }
        public void Update()
        {
            InternalCalls.AudioComponent_Update(_Actor.ID);
        }
    }

    public class Mesh3DComponent : Component
    {
        public string File
        {
            get
            {
                InternalCalls.Mesh3DComponent_GetFile(_Actor.ID, out string _f);
                return _f;
            }
            set
            {
                InternalCalls.Mesh3DComponent_SetFile(_Actor.ID, value);
            }
        }
        public bool Visible
        {
            get
            {
                return true;
            }
            set
            {
                InternalCalls.LogNoImpl();
            }
        }
        public void Apply()
        {
            InternalCalls.Mesh3DComponent_Load(_Actor.ID);
        }
    }

    public class BillboardComponent : Component
    {
        public string File
        {
            get
            {
                InternalCalls.BillboardComponent_GetFile(_Actor.ID, out string _f);
                return _f;
            }
            set
            {
                InternalCalls.BillboardComponent_SetFile(_Actor.ID, value);
            }
        }
        public void Apply()
        {
            InternalCalls.BillboardComponent_Load(_Actor.ID);
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
        public float Intensity
        {
            get
            {
                InternalCalls.PointlightComponent_GetIntensity(_Actor.ID, out float _intensity);
                return _intensity;
            }
            set
            {
                InternalCalls.PointlightComponent_SetIntensity(_Actor.ID, ref value);
            }
        }
        public float Specular
        {
            get
            {
                InternalCalls.PointlightComponent_GetSpecularFactor(_Actor.ID, out float _specular);
                return _specular;
            }
            set
            {
                InternalCalls.PointlightComponent_SetSpecularFactor(_Actor.ID, ref value);
            }
        }
        public Vector3 AttenuationFactor
        {
            get
            {
                InternalCalls.PointlightComponent_GetAttenuationFactor(_Actor.ID, out Vector3 _af);
                return _af;
            }
            set
            {
                InternalCalls.PointlightComponent_SetAttenuationFactor(_Actor.ID, ref value);
            }
        }
    }
    public class SpotlightComponent : Component
    {
        public Vector3 Color
        {
            get
            {
                InternalCalls.SpotlightComponent_GetColor(_Actor.ID, out Vector3 _color);
                return _color;
            }
            set
            {
                InternalCalls.SpotlightComponent_SetColor(_Actor.ID, ref value);
            }
        }
        public float Intensity
        {
            get
            {
                InternalCalls.SpotlightComponent_GetIntensity(_Actor.ID, out float _intensity);
                return _intensity;
            }
            set
            {
                InternalCalls.SpotlightComponent_SetIntensity(_Actor.ID, ref value);
            }
        }
        public float Specular
        {
            get
            {
                InternalCalls.SpotlightComponent_GetSpecularFactor(_Actor.ID, out float _specular);
                return _specular;
            }
            set
            {
                InternalCalls.SpotlightComponent_SetSpecularFactor(_Actor.ID, ref value);
            }
        }
        public float OuterAngle
        {
            get
            {
                InternalCalls.SpotlightComponent_GetOuterAngle(_Actor.ID, out float _angle);
                return _angle;
            }
            set
            {
                InternalCalls.SpotlightComponent_SetOuterAngle(_Actor.ID, ref value);
            }
        }
        public float InnerAngle
        {
            get
            {
                InternalCalls.SpotlightComponent_GetInnerAngle(_Actor.ID, out float _angle);
                return _angle;
            }
            set
            {
                InternalCalls.SpotlightComponent_SetInnerAngle(_Actor.ID, ref value);
            }
        }
        public Vector3 AttenuationFactor
        {
            get
            {
                InternalCalls.SpotlightComponent_GetAttenuationFactor(_Actor.ID, out Vector3 _af);
                return _af;
            }
            set
            {
                InternalCalls.SpotlightComponent_SetAttenuationFactor(_Actor.ID, ref value);
            }
        }
    }
    public class DirectionalLightComponent : Component
    {
        public Vector3 Color
        {
            get
            {
                InternalCalls.DirectionalLightComponent_GetColor(_Actor.ID, out Vector3 _color);
                return _color;
            }
            set
            {
                InternalCalls.DirectionalLightComponent_SetColor(_Actor.ID, ref value);
            }
        }
        public float Intensity
        {
            get
            {
                InternalCalls.DirectionalLightComponent_GetIntensity(_Actor.ID, out float _intensity);
                return _intensity;
            }
            set
            {
                InternalCalls.DirectionalLightComponent_SetIntensity(_Actor.ID, ref value);
            }
        }
        public float Specular
        {
            get
            {
                InternalCalls.DirectionalLightComponent_GetSpecularFactor(_Actor.ID, out float _specular);
                return _specular;
            }
            set
            {
                InternalCalls.DirectionalLightComponent_SetSpecularFactor(_Actor.ID, ref value);
            }
        }
    }
}