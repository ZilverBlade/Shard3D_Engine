using Shard3D.Core;
using static Shard3D.Core.InternalCalls;

namespace Shard3D.UI
{
    class Example : HUD
    {
        private bool isPlaying = false;
        protected void HoverEvent(float dt)
        {
            Log("I'm being hovered over!");
        }

        protected void PressEvent(float dt)
        {
            Log("I'm being pressed on!");
        }
       
        protected void ClickEvent()
        {
            if (isPlaying) { Log("Hey chill!"); return; }
            Actor actor = new Actor(2784833615538243943);
            actor.AddComponent<Components.AudioComponent>();
            actor.GetComponent<Components.AudioComponent>().File = "assets/audiodata/thou-3.mp3";
            actor.GetComponent<Components.AudioComponent>().Play();
            isPlaying = true;
            SceneManager.DestructHUDLayer(0);
        }
    }
}
