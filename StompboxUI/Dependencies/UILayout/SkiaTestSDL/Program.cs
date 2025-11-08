using SDL2;
using UILayout;
using UILayout.Test;

namespace SkiaTestSDL
{
    internal class Program
    {
        static LayoutWindow window;

        static void Main(string[] args)
        {
            window = new LayoutWindow("SkiaTestSDL", 1080, 800);

            SkiaLayout ui = new SkiaLayout();

            ui.RootUIElement = new LayoutTest();

            window.SetLayout(ui);

            window.Run();
        }
    }
}
