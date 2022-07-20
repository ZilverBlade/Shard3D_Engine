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
    }
}
