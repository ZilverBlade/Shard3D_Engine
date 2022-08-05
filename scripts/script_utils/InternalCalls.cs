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
        public extern static void SpotlightComponent_GetIntensity(ulong GUID, out float _i);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetAttenuationFactor(ulong GUID, out Vector3 _c);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetSpecularFactor(ulong GUID, out float _s);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetRadius(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetInnerAngle(ulong GUID, out float _r);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SpotlightComponent_GetOuterAngle(ulong GUID, out float _r);
        #endregion

    }
}
