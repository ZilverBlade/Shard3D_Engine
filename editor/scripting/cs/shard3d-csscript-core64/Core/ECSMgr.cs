namespace Shard3D.Core
{
    public class ECS
    {
        public static Actor SpawnActor(string name = "Some kind of Actor")
        {
            InternalCalls.SpawnActor(out ulong guid, name);
            return new Actor(guid);
        }
        public static Actor GetActor(ulong guid)
        {
            return new Actor(guid);
        }
        public static Actor GetActor(string tag)
        {
            InternalCalls.GetActorByTag(out ulong guid, tag);
            return new Actor(guid);
        }
        public static void KillActor(ulong guid)
        {
            InternalCalls.KillActor(guid);
        }
    }
}
