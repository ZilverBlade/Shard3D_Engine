using System.Runtime.CompilerServices;

namespace Shard3D.Core
{
    public static class InternalCalls
    {
        #region Logging

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Log(string message, LogSeverity severity = LogSeverity.Debug);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void LogNoImpl();

        #endregion

        #region ECS

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpawnActor(out ulong guid, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Actor_HasComponent(ulong guid, System.Type componentType, int lang);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Actor_AddComponent(ulong guid, System.Type componentType, int lang);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Actor_RmvComponent(ulong guid, System.Type componentType, int lang);
        public static void SpawnBlueprint() => LogNoImpl();

        #endregion

        #region SceneManager

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SceneManager_PossessCameraActor(ulong guid);

        #endregion

        #region Transforms

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformComponent_GetTranslation(ulong guid, out Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformComponent_SetTranslation(ulong guid, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformComponent_GetRotation(ulong guid, out Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformComponent_SetRotation(ulong guid, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformComponent_GetScale(ulong guid, out Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformComponent_SetScale(ulong guid, ref Vector3 scale);

        #endregion

        #region CameraComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_GetFOV(ulong GUID, out float _f);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_SetFOV(ulong GUID, ref float _f);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_GetProjectionType(ulong GUID, out int _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_SetProjectionType(ulong GUID, ref int _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_GetNearClip(ulong GUID, out float _nc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_SetNearClip(ulong GUID, ref float _nc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_GetFarClip(ulong GUID, out float _fc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CameraComponent_SetFarClip(ulong GUID, ref float _fc);

        #endregion

        #region AudioComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_GetFile(ulong GUID, out string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_SetFile(ulong GUID, string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_GetPropertiesVolume(ulong GUID, out float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]                  
        public extern static void AudioComponent_SetPropertiesVolume(ulong GUID, ref float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_GetPropertiesPitch(ulong GUID, out float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_SetPropertiesPitch(ulong GUID, ref float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_Play(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_Pause(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_Stop(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_Resume(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioComponent_Update(ulong GUID);

        #endregion

        #region MeshComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MeshComponent_GetFile(ulong GUID, out string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MeshComponent_SetFile(ulong GUID, string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MeshComponent_Load(ulong GUID);

        #endregion

        #region BillboardComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void BillboardComponent_GetFile(ulong GUID, out string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void BillboardComponent_SetFile(ulong GUID, string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void BillboardComponent_Load(ulong GUID);

        #endregion

        #region PointlightComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_GetColor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_SetColor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_GetIntensity(ulong GUID, out float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_SetIntensity(ulong GUID, ref float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_GetAttenuationFactor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_SetAttenuationFactor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_GetSpecularFactor(ulong GUID, out float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_SetSpecularFactor(ulong GUID, ref float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_GetRadius(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PointlightComponent_SetRadius(ulong GUID, ref float _r);

        #endregion

        #region SpotlightComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetColor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_SetColor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetIntensity(ulong GUID, out float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_SetIntensity(ulong GUID, ref float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetAttenuationFactor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_SetAttenuationFactor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetSpecularFactor(ulong GUID, out float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_SetSpecularFactor(ulong GUID, ref float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetRadius(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_SetRadius(ulong GUID, ref float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetInnerAngle(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_SetInnerAngle(ulong GUID, ref float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetOuterAngle(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_SetOuterAngle(ulong GUID, ref float _r);

        #endregion

        #region DirectionalLightComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void DirectionalLightComponent_GetColor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void DirectionalLightComponent_SetColor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void DirectionalLightComponent_GetIntensity(ulong GUID, out float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void DirectionalLightComponent_SetIntensity(ulong GUID, ref float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void DirectionalLightComponent_GetSpecularFactor(ulong GUID, out float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void DirectionalLightComponent_SetSpecularFactor(ulong GUID, ref float _s);

        #endregion

    }
}
