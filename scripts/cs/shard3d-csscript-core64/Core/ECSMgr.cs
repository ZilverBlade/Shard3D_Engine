namespace Shard3D.Core
{
    public class ECS
    {
        public static Actor SpawnActor(string name = "Some kind of Actor")
        {
            InternalCalls.SpawnActor(out ulong guid, name);
            return new Actor(guid);
        }
    }
}
