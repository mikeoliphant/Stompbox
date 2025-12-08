using System;
using UILayout;
using StompboxAPI;

namespace Stompbox
{
    public class StompboxLayout : SkiaLayout
    {
        public static StompboxLayout Instance { get; private set; }
        public static float BaseScale { get; set; } = 1.0f;

        bool clientConnected = true;

        public StompboxLayout()
        {
            Instance = this;

            InterfaceBase.SetInterfaceType(InterfaceBase.InterfaceType);
        }

        public void SetReferenceHeight(float height)
        {
        }

        public void Connect()
        {
            Console.WriteLine("Connecting to Stompbox");

            clientConnected = false;

            string serverName = "localhost";

            StompboxClient.Instance.Connect(serverName, 24639, ConnectCallback);
        }

        void ConnectCallback(bool connected)
        {
            if (!connected)
            {
                Console.WriteLine("Unable to connect to host");
            }
            else
            {
                Console.WriteLine("Connected");

                clientConnected = true;
            }
        }

        public override void Update(float secondsElapsed)
        {
            base.Update(secondsElapsed);

            if (StompboxClient.Instance.InClientMode)
            {
                if (clientConnected && !StompboxClient.Instance.Connected)
                {
                    Connect();
                }
            }
        }
    }
}