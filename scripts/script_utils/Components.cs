namespace Shard3D.Core.Components
{
    public struct TransformComponent
    {
        public Vector3 translation;
        public Vector3 rotation;
        public Vector3 scale;// = new Vector3(1.0F, 1.0F, 1.0F);
    }
    public struct GUIDComponent
    {
        private ulong id;
        public ulong GetID()
        {
            return id;
        }
    }

    public struct TagComponent
    {
        public string tag;

        public static implicit operator TagComponent(string value)
        {
            TagComponent tagc = new TagComponent
            {
                tag = value
            };
            return tagc;
        }
    }
}

