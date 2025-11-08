using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Xml.Serialization;
using UILayout;

namespace MonoGameTest
{
    public class TestGameHost : MonoGameHost
    {
        public TestGameHost(int screenWidth, int screenHeight, bool isFullscreen)
            : base(screenWidth, screenHeight, isFullscreen)
        {
        }

        protected override void LoadContent()
        {
            Layout.RootUIElement = new UILayout.Test.LayoutTest();
        }
    }
}