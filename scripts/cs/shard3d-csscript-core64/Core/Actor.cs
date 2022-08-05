using System;

namespace Shard3D.Core
{
    public class Actor
    {
        protected Actor() { ID = 1; }
        internal Actor(ulong _id) { ID = _id; }

        public readonly ulong ID;

        #region ECS
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Actor_HasComponent(ID, componentType);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>()) return null;
            T component = new T() { _Actor = this };
            return component;
        }

        #endregion


    }
}

