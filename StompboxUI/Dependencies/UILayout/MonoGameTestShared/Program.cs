using System;

namespace UILayout
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            using var host = new MonoGameTest.TestGameHost(800, 600, isFullscreen: false);

            host.UsePremultipliedAlpha = false; // Because our embedded assets are not pre-multiplied

            MonoGameLayout layout = new MonoGameLayout();

            host.StartGame(layout);
        }
    }
}