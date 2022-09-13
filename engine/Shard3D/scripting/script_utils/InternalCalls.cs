﻿using System.Runtime.CompilerServices;

namespace Shard3D.Core
{
    internal static class InternalCalls
    {
        #region Logging

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log(string message, LogSeverity severity = LogSeverity.Debug);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogNoImpl();

        #endregion

        #region ECS

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpawnActor(out ulong guid, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GetActorByTag(out ulong guid, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void KillActor(ulong guid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Actor_HasComponent(ulong guid, System.Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Actor_AddComponent(ulong guid, System.Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Actor_RmvComponent(ulong guid, System.Type componentType);
        internal static void SpawnBlueprint() => LogNoImpl();

        #endregion

        #region SceneManager

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManager_PossessCameraActor(ulong guid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManagerLoadLevel(string levelPath);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManagerLoadHUD(string hudTemplatePath, int layer);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManagerDestroyHUDLayer(int layer);

        #endregion

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool IsKeyDown(KeyInput keyCode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool IsMouseButtonDown(MouseInput mouseButton);

        #region Transforms

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong guid, out Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong guid, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong guid, out Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong guid, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong guid, out Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong guid, ref Vector3 scale);

        #endregion

        #region CameraComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetFOV(ulong GUID, out float _f);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetFOV(ulong GUID, ref float _f);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetProjectionType(ulong GUID, out int _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetProjectionType(ulong GUID, ref int _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetNearClip(ulong GUID, out float _nc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetNearClip(ulong GUID, ref float _nc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetFarClip(ulong GUID, out float _fc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetFarClip(ulong GUID, ref float _fc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPostProcessingEffectsCount(ulong GUID, out uint _count);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPostProcessingParametersCount(ulong GUID, uint _index, out uint _count);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPostProcessingParameterValue_int(ulong GUID, uint _effect_index, uint _param_index, out int _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetPostProcessingParameterValue_int(ulong GUID, uint _effect_index, uint _param_index, ref int _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPostProcessingParameterValue_float(ulong GUID, uint _effect_index, uint _param_index, out float _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetPostProcessingParameterValue_float(ulong GUID, uint _effect_index, uint _param_index, ref float _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPostProcessingParameterValue_float2(ulong GUID, uint _effect_index, uint _param_index, out Vector2 _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetPostProcessingParameterValue_float2(ulong GUID, uint _effect_index, uint _param_index, ref Vector2 _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPostProcessingParameterValue_float4(ulong GUID, uint _effect_index, uint _param_index, out Vector4 _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetPostProcessingParameterValue_float4(ulong GUID, uint _effect_index, uint _param_index, ref Vector4 _value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPostProcessingParameterValueType(ulong GUID, uint _effect_index, uint _param_index, out System.Type _value);

        #endregion

        #region AudioComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_GetFile(ulong GUID, out string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_SetFile(ulong GUID, string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_GetPropertiesVolume(ulong GUID, out float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_SetPropertiesVolume(ulong GUID, ref float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_GetPropertiesPitch(ulong GUID, out float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_SetPropertiesPitch(ulong GUID, ref float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_Play(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_Pause(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_Stop(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_Resume(ulong GUID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioComponent_Update(ulong GUID);

        #endregion

        #region Mesh3DComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Mesh3DComponent_GetFile(ulong GUID, out string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Mesh3DComponent_SetFile(ulong GUID, string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Mesh3DComponent_Load(ulong GUID);

        #endregion

        #region BillboardComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BillboardComponent_GetFile(ulong GUID, out string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BillboardComponent_SetFile(ulong GUID, string str);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BillboardComponent_Load(ulong GUID);

        #endregion

        #region PointlightComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_GetColor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_SetColor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_GetIntensity(ulong GUID, out float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_SetIntensity(ulong GUID, ref float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_GetAttenuationFactor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_SetAttenuationFactor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_GetSpecularFactor(ulong GUID, out float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_SetSpecularFactor(ulong GUID, ref float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_GetRadius(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointlightComponent_SetRadius(ulong GUID, ref float _r);

        #endregion

        #region SpotlightComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_GetColor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_SetColor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_GetIntensity(ulong GUID, out float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_SetIntensity(ulong GUID, ref float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_GetAttenuationFactor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_SetAttenuationFactor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_GetSpecularFactor(ulong GUID, out float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_SetSpecularFactor(ulong GUID, ref float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_GetRadius(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_SetRadius(ulong GUID, ref float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_GetInnerAngle(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_SetInnerAngle(ulong GUID, ref float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_GetOuterAngle(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotlightComponent_SetOuterAngle(ulong GUID, ref float _r);

        #endregion

        #region DirectionalLightComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DirectionalLightComponent_GetColor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DirectionalLightComponent_SetColor(ulong GUID, ref Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DirectionalLightComponent_GetIntensity(ulong GUID, out float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DirectionalLightComponent_SetIntensity(ulong GUID, ref float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DirectionalLightComponent_GetSpecularFactor(ulong GUID, out float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DirectionalLightComponent_SetSpecularFactor(ulong GUID, ref float _s);

        #endregion

    }
}
