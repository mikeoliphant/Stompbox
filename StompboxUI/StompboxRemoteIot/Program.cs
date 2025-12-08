using Iot.Device.Graphics.SkiaSharpAdapter;
using Stompbox;
using StompboxAPI;
using System;
using System.Device.Gpio;
using System.Device.Spi;
using UILayout;

namespace StompboxRemoteIot
{
    internal class Program
    {
        static void Main(string[] args)
        {
            SkiaLayout.DefaultTextureNamespace = "StompboxRemoteIot";

            RemoteClient client = new RemoteClient();

            //client.MidiCallback = SendMidiCommand;

            InterfaceBase.InterfaceType = EStompboxInterfaceType.Pedalboard;

            StompboxLayout layout = new StompboxLayout();

            int pinDC = 6;
            int pinReset = 5;
            int pinLed = -1;

            SkiaSharpAdapter.Register();

            using SpiDevice displaySPI = SpiDevice.Create(new SpiConnectionSettings(0, 0) { ClockFrequency = 24000000, Mode = SpiMode.Mode3 });

            using GpioController gpio = new();

            Console.WriteLine("Open display");

            using var ili9341 = new Ili9341(displaySPI, pinDC, pinReset, backlightPin: pinLed, gpioController: gpio);

            using var backBuffer = ili9341.GetBackBufferCompatibleImage();

            var api = backBuffer.GetDrawingApi();

            layout.GraphicsContext.Canvas = api.GetCanvas();

            layout.SetBounds(new RectF(0, 0, backBuffer.Width, backBuffer.Height));

            do
            {
                Console.WriteLine("Draw");

                layout.UpdateLayout();

                layout.Update(0.1f);

                layout.Draw();
                ili9341.DrawBitmap(backBuffer);

                ili9341.SendFrame(false);
            }
            while (true);
        }
    }
}
