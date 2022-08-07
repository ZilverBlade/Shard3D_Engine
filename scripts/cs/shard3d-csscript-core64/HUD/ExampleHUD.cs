using Shard3D.Core;
using static Shard3D.Core.InternalCalls;

namespace Shard3D.UI
{
    class Example : HUD
    {
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
            SceneManager.LoadLevel("assets/leveldata/");
        }
    }
}
