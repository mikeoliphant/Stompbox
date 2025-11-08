using SkiaSharp;
using System.Windows;
using UILayout;
using UILayout.Skia.WPF;
using UILayout.Test;

namespace SkiaTest
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            LayoutWindow layoutWindow = new LayoutWindow()
            {
                Width = 1024,
                Height = 800
            };

            SkiaLayout ui = new SkiaLayout();

            ui.RootUIElement = new LayoutTest();

            layoutWindow.SkiaCanvas.SetLayout(ui);

            layoutWindow.Show();
        }
    }
}
