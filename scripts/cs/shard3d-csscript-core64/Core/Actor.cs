namespace Shard3D.Core
{
    public class Actor
    {
        protected Actor()  {  ID = 1;  }
        internal Actor(ulong _id) { ID = _id; }

        public readonly ulong ID;
#region Transform
        public Vector3 Translation
        {
            get
            {
                InternalCalls.GetTranslation(ID, out Vector3 _translation);
                return _translation;
            }
            set
            {
                InternalCalls.SetTranslation(ID, ref value);
            }
        }
        public Vector3 Rotation
        {
            get
            {
                InternalCalls.GetRotation(ID, out Vector3 _rotation);
                return _rotation;
            }
            set
            {
                InternalCalls.SetRotation(ID, ref value);
            }
        }
        public Vector3 Scale
        {
            get
            {
                InternalCalls.GetScale(ID, out Vector3 _scale);
                return _scale;
            }
            set
            {
                InternalCalls.SetScale(ID, ref value);
            }
        }
#endregion


#region ECS
        public void AddComponent(Components _component)
        {
            InternalCalls.ActorAddComponent(ID, _component);
        }
        public void RmvComponent(Components _component)
        {
            InternalCalls.ActorRmvComponent(ID, _component);
        }
#endregion


    }
}

