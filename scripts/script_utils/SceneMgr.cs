namespace Shard3D.Core
{
    public static class SceneManager
    {
        public static void LoadLevel(string levelPath)
        {
            InternalCalls.SceneManagerLoadLevel(levelPath);
        }
        public static void LoadHUDTemplate(string hudTemplatePath, int layer)
        {
            InternalCalls.SceneManagerLoadHUD(hudTemplatePath, layer);
        }
        public static void DestructHUDLayer(int layer)
        {
            InternalCalls.SceneManagerDestroyHUDLayer(layer);
        }
    }
}
