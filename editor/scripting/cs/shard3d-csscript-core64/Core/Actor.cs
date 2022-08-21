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
            return InternalCalls.Actor_HasComponent(ID, componentType, 0);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>()) { InternalCalls.Log("Tried to get component that does not exist!", LogSeverity.Error); return null; }
            T component = new T() { _Actor = this };
            return component;
        }
        public T AddComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>()) { InternalCalls.Log("Tried to add existing component!", LogSeverity.Warn); return null; }
            Type componentType = typeof(T);
            InternalCalls.Actor_AddComponent(ID, componentType, 0);
            T component = new T() { _Actor = this };
            return component;
        }
        public void KillComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>()) { InternalCalls.Log("Tried to remove component that does not exist!", LogSeverity.Warn); return; }
            Type componentType = typeof(T);
            InternalCalls.Actor_RmvComponent(ID, componentType, 0);
        }
        #endregion


    }
}

