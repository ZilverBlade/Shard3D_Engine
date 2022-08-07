using System.Runtime.CompilerServices;

[assembly: InternalsVisibleTo("Shard3D.Core.InternalCalls")]
namespace Shard3D.Core
{
    public enum LogSeverity
    {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3
    }
    public enum CameraProjectionType
    {
        Orthographic = 0,
        Perspective = 1
    }
    public struct AudioProperties
    {
        public float volume;
        public float pitch;
        public Vector3 relativePos;
    }
}