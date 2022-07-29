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
        public extern static void GetTranslation(ulong guid, out Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetTranslation(ulong guid, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetRotation(ulong guid, out Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetRotation(ulong guid, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetScale(ulong guid, out Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetScale(ulong guid, ref Vector3 scale);
#endregion
    }
}
