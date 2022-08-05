namespace Shard3D.Core
{
    public class HUD
    {
        protected HUD()  {  ID = 1;  }
        internal HUD(ulong _id) { ID = _id; }

        public readonly ulong ID;

#region Transform
        public Vector2 Position
        {
            get
            {
                return new Vector2();
            }
            set
            {

            }
        }
        public float Rotation
        {
            get
            {
                return new float();
            }
            set
            {

            }
        }
        public Vector2 Scale
        {
            get
            {
                return new Vector2();
            }
            set
            {

            }
        }
#endregion


#region Texture
        public string DefaultTexture
        {
            get
            {
                return "";
            }
            set
            {

            }
        }
        public string HoverTexture
        {
            get
            {
                return "";
            }
            set
            {

            }
        }
        public string PressTexture
        {
            get
            {
                return "";
            }
            set
            {

            }
        }
#endregion
    }
}

