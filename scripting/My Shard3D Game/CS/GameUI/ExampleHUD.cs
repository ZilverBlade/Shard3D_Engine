using Shard3D.Core;
namespace Shard3D.UI
{
    class Example : HUD
    {
        private bool isPlaying = false;
        protected void HoverEvent(float dt)
        {
            
        }

        protected void PressEvent(float dt)
        {
        }
       
        protected void ClickEvent()
        {
            if (isPlaying) { return; }
            Actor actor = ECS.SpawnActor("Audio Actor");
            actor.AddComponent<Components.AudioComponent>();
            actor.GetComponent<Components.AudioComponent>().File = "assets/audiodata/thou-3.mp3";
            actor.GetComponent<Components.AudioComponent>().Play();
            isPlaying = true;
            SceneManager.DestructHUDLayer(0);
        }
    }
}
