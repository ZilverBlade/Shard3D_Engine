namespace Shard3D.Core
{
    public class HUD
    {
        protected HUD()  {  ID = 1;  }
        internal HUD(ulong _id) { ID = _id; }

        public readonly ulong ID;
    }
}

