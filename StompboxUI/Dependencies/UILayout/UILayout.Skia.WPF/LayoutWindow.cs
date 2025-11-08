using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;

namespace UILayout.Skia.WPF
{
    public class LayoutWindow : Window
    {
        [DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll")]
        public static extern bool GetWindowRect(IntPtr hwnd, ref Rectangle rectangle);

        public LayoutControl SkiaCanvas { get; private set; }
        public float ScreenDPI { get; private set; }

        public Rectangle DisplayRectangle
        {
            get
            {
                Rectangle displayRectangle = new Rectangle();

                GetWindowRect(parentWindow, ref displayRectangle);

                return displayRectangle;
            }
        }

        IntPtr parentWindow;

        public LayoutWindow()
        {
            Content = SkiaCanvas = new LayoutControl();
        }

        public void SetSize(uint width, uint height)
        {
            Width = width;
            Height = height;
        }

        public void Show()
        {
            Show(IntPtr.Zero);
        }

        public void Show(IntPtr parentWindow)
        {
            this.parentWindow = parentWindow;

            //Content = EditorView;

            base.Show();

            if (parentWindow != IntPtr.Zero)
            {
                Top = 0;
                Left = 0;
                ShowInTaskbar = false;
                WindowStyle = System.Windows.WindowStyle.None;
                ResizeMode = System.Windows.ResizeMode.NoResize;

                var windowHwnd = new System.Windows.Interop.WindowInteropHelper(this);
                IntPtr hWnd = windowHwnd.Handle;
                SetParent(hWnd, parentWindow);
            }

            var m = PresentationSource.FromVisual(this).CompositionTarget.TransformToDevice;

            float scaleX = (float)m.M11;
            float scaleY = (float)m.M22;

            ScreenDPI = Math.Max(scaleX, scaleY);
        }
    }
}
